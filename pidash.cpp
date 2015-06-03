#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* web_view, GtkWidget* window);
static gboolean keyPressCb(GtkWidget * widget, GdkEvent * event, gpointer data);

static void next_page();
static void previous_page();

static void detach_current_page();
static void attach_current_page();

int current_page_index = 0;
gulong key_handler;
const int NUM_URLS = 3;
char *urls[] = {
  "file:///home/daniel/pidash/page1.html",
  "file:///home/daniel/pidash/page2.html",
  "file:///home/daniel/pidash/page3.html"
};
WebKitWebView *web_views[NUM_URLS];
GtkWidget *main_window;

int main(int argc, char* argv[])
{
  // Initialize GTK+
  gtk_init(&argc, &argv);

  // Create an 800x600 window that will contain the browser instance
  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

  // Create a browser instance
  for (int i = 0; i < NUM_URLS; i++) {
    web_views[i] = WEBKIT_WEB_VIEW(g_object_ref(webkit_web_view_new()));
    g_signal_connect(web_views[i], "close", G_CALLBACK(closeWebViewCb), NULL);
    webkit_web_view_load_uri(web_views[i], urls[i]);
  }

  // Put the browser area into the main window
  gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(web_views[0]));

  // Set up callbacks so that if either the main window or the browser instance is
  // closed, the program will exit
  g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
  key_handler = g_signal_connect(web_views[0], "key-press-event", G_CALLBACK(keyPressCb), main_window);

  // Make sure that when the browser area becomes visible, it will get mouse
  // and keyboard events
  gtk_widget_grab_focus(GTK_WIDGET(web_views[0]));

  // Make sure the main window and all its contents are visible
  gtk_widget_show_all(main_window);

  // Run the main GTK+ event loop
  gtk_main();

  return 0;
}

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
  gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* web_view, GtkWidget* window)
{
  gtk_widget_destroy(window);
  return TRUE;
}

static gboolean keyPressCb(GtkWidget * widget, GdkEvent * event, gpointer data)
{
  GdkEventKey key = event->key;

  switch (key.keyval) {
    case GDK_KEY_Escape:
      gtk_main_quit();
      return TRUE;
      break;
    case GDK_KEY_Left:
      previous_page(); break;
    case GDK_KEY_Right:
      next_page(); break;
    default: break;
  }
}

static void next_page()
{
  detach_current_page();
  current_page_index += 1;
  if (current_page_index >= NUM_URLS) current_page_index = 0;
  attach_current_page();
}

static void previous_page()
{
  detach_current_page();
  current_page_index -= 1;
  if (current_page_index < 0) current_page_index = NUM_URLS - 1;
  attach_current_page();
}

static void detach_current_page()
{
  g_signal_handler_disconnect(web_views[current_page_index], key_handler);
  gtk_container_remove(GTK_CONTAINER(main_window), GTK_WIDGET(web_views[current_page_index]));
}

static void attach_current_page()
{
  gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(web_views[current_page_index]));
  key_handler = g_signal_connect(web_views[current_page_index], "key-press-event", G_CALLBACK(keyPressCb), main_window);
  gtk_widget_grab_focus(GTK_WIDGET(web_views[current_page_index]));
  gtk_widget_show_all(main_window);
}
