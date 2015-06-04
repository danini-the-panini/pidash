#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <ios>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <vte/vte.h>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

static vector<string> split(const string &s, char delim);
static char** to_argv(const vector<string>& args);
static long get_file_size(const string &filename);

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* web_view, GtkWidget* window);
static gboolean keyPressCb(GtkWidget * widget, GdkEvent * event, gpointer data);

static GtkWidget* load_url(const string& url);
static GtkWidget* load_file(const string& path);
static GtkWidget* load_shell(const vector<string>& args);

static void next_page();
static void previous_page();

static void detach_current_page();
static void attach_current_page();

static WebKitWebView* create_web_view();
static GtkWidget* wrap_in_scroll(GtkWidget* widget);
static void add_widget(GtkWidget* widget);

static void get_screen_size();
static void create_window();
static void load_setup_file();

int current_page_index = 0;
gulong key_handler;
GtkWidget *main_window;

int screen_width;
int screen_height;

vector<GtkWidget*> page_views;

int main(int argc, char* argv[])
{
  gtk_init(&argc, &argv);

  get_screen_size();
  create_window();
  load_setup_file();

  attach_current_page();

  gtk_main();

  return 0;
}

static void get_screen_size()
{
  GdkScreen* screen = gdk_screen_get_default();
  screen_width = gdk_screen_get_width(screen);
  screen_height = gdk_screen_get_height(screen);
}

static void create_window()
{
  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), screen_width, screen_height);
  gtk_window_fullscreen(GTK_WINDOW(main_window));
  g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
  key_handler = g_signal_connect(main_window, "key-press-event", G_CALLBACK(keyPressCb), main_window);
}

static void load_setup_file()
{
  ifstream input( "pages.txt" );
  for( string line; getline( input, line ); ) {
    vector<string> tokens = split(line, ' ');
    if(tokens[0].compare("url") == 0) {
      add_widget(load_url(tokens[1]));
    } else if (tokens[0].compare("file") == 0) {
      add_widget(load_file(tokens[1]));
    } else if (tokens[0].compare("shell") == 0) {
      tokens.erase(tokens.begin());
      add_widget(load_shell(tokens));
    }
  }
}

static void add_widget(GtkWidget* widget)
{
  gtk_widget_set_halign(widget, GTK_ALIGN_FILL);
  gtk_widget_set_valign(widget, GTK_ALIGN_FILL);
  page_views.push_back(GTK_WIDGET(g_object_ref(widget)));
}

static GtkWidget* wrap_in_scroll(GtkWidget* widget)
{
  GtkWidget* scroll_widget = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scroll_widget), widget);
  return scroll_widget;
}

static WebKitWebView* create_web_view()
{
  WebKitWebView* web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
  WebKitWebWindowFeatures *features = webkit_web_view_get_window_features(web_view);

  return web_view;
}

static GtkWidget* load_url(const string& url)
{
  cout << "Loading URL " << url << endl;

  WebKitWebView* web_view = create_web_view();
  webkit_web_view_load_uri(web_view, url.c_str());
  return wrap_in_scroll(GTK_WIDGET(web_view));
}

static GtkWidget* load_file(const string& path)
{
  cout << "Loading FILE " << path << endl;

  long size = get_file_size(path);
  char *buffer = new char[size + 1];
  ifstream t(path.c_str());
  t.read(buffer, size);
  buffer[size] = 0;
  t.close();

  WebKitWebView* web_view = create_web_view();
  webkit_web_view_load_string(web_view, buffer, NULL, NULL, NULL);
  return wrap_in_scroll(GTK_WIDGET(web_view));
}

static GtkWidget* load_shell(const vector<string>& args)
{
  cout << "Loading Shell Command:";
  for (size_t i = 0; i < args.size(); i++) {
    cout << " " << args[i];
  }
  cout << endl;

  char** argv = to_argv(args);
  GError* err = NULL; GPid pid;

  VteTerminal* term_view = VTE_TERMINAL(g_object_ref(vte_terminal_new()));
  vte_terminal_spawn_sync(term_view, VTE_PTY_DEFAULT, 0, argv, 0, (GSpawnFlags)(G_SPAWN_SEARCH_PATH), 0, 0, &pid, NULL, &err);
  if (err) cout << "***** ERR: " << err->code << ", " << "MSG: " << err->message << endl;

  delete [] argv;
  return GTK_WIDGET(term_view);
}

static char** to_argv(const vector<string>& args)
{
  char ** argv = new char*[args.size() + 1];
  for (int i = 0; i < args.size(); i++) {
    argv[i] = new char[args[i].size() + 1];
    strncpy(argv[i], args[i].c_str(), args[i].size() + 1);
  }
  argv[args.size()] = 0;
  return argv;
}

static long get_file_size(const string &filename)
{
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
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
  if (current_page_index >= page_views.size()) current_page_index = 0;
  attach_current_page();
}

static void previous_page()
{
  detach_current_page();
  current_page_index -= 1;
  if (current_page_index < 0) current_page_index = page_views.size() - 1;
  attach_current_page();
}

static void detach_current_page()
{
  GtkWidget* current_page_view = page_views[current_page_index];
  g_signal_handler_disconnect(current_page_view, key_handler);
  gtk_container_remove(GTK_CONTAINER(main_window), current_page_view);
}

static void attach_current_page()
{
  GtkWidget* current_page_view = page_views[current_page_index];
  gtk_container_add(GTK_CONTAINER(main_window), current_page_view);
  gtk_widget_show_all(main_window);
}

static vector<string> split(const string &s, char delim)
{
  vector<string> elems; stringstream ss(s); string item;
  while (getline(ss, item, delim)) elems.push_back(item);
  return elems;
}
