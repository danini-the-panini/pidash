#include <iostream>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

static vector<string> split(const string &s, char delim);

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* web_view, GtkWidget* window);
static gboolean keyPressCb(GtkWidget * widget, GdkEvent * event, gpointer data);

static void next_page();
static void previous_page();

static void detach_current_page();
static void attach_current_page();

int current_page_index = 0;
gulong key_handler;
GtkWidget *main_window;

vector<string> urls;
vector<WebKitWebView*> web_views;

int main(int argc, char* argv[])
{
  // Initialize GTK+
  gtk_init(&argc, &argv);

  // Create an 800x600 window that will contain the browser instance
  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

  ifstream input( "pages.txt" );
  for( string line; getline( input, line ); ) {
    vector<string> tokens = split(line, ' ');
    if(tokens[0].compare("url") == 0) {
      urls.push_back(tokens[1]);
    }
  }

  // Create a browser instance
  for (int i = 0; i < urls.size(); i++) {
    WebKitWebView* web_view = WEBKIT_WEB_VIEW(g_object_ref(webkit_web_view_new()));
    // g_signal_connect(web_view, "close", G_CALLBACK(closeWebViewCb), NULL);
    webkit_web_view_load_uri(web_view, urls[i].c_str());
    web_views.push_back(web_view);
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
  return TRUE;
}

static void next_page()
{
  detach_current_page();
  current_page_index += 1;
  if (current_page_index >= urls.size()) current_page_index = 0;
  attach_current_page();
}

static void previous_page()
{
  detach_current_page();
  current_page_index -= 1;
  if (current_page_index < 0) current_page_index = urls.size() - 1;
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

static vector<string> split(const string &s, char delim)
{
  vector<string> elems; stringstream ss(s); string item;
  while (getline(ss, item, delim)) elems.push_back(item);
  return elems;
}
