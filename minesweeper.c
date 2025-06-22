#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 8
#define COLS 8
#define MINES 10

typedef struct {
    gboolean is_mine;
    gboolean is_revealed;
    gboolean is_flagged;
    int surrounding;
    GtkWidget *button;
} Cell;

Cell cells[ROWS][COLS];
GtkWidget *window;

int count_mines(int x, int y) {
    int c = 0;
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS)
                if (cells[nx][ny].is_mine) c++;
        }
    return c;
}

void game_over() {
    for (int x = 0; x < ROWS; x++)
        for (int y = 0; y < COLS; y++)
            if (cells[x][y].is_mine)
                gtk_button_set_label(GTK_BUTTON(cells[x][y].button), "💣");
    GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "💥 Game Over!");
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
}

void reveal(int x, int y) {
    if (cells[x][y].is_revealed || cells[x][y].is_flagged) return;
    cells[x][y].is_revealed = TRUE;
    gtk_widget_set_sensitive(cells[x][y].button, FALSE);

    if (cells[x][y].is_mine) {
        gtk_button_set_label(GTK_BUTTON(cells[x][y].button), "💣");
        game_over();
        return;
    }

    int n = cells[x][y].surrounding;
    if (n > 0) {
        char s[2]; sprintf(s, "%d", n);
        gtk_button_set_label(GTK_BUTTON(cells[x][y].button), s);
    } else {
        gtk_button_set_label(GTK_BUTTON(cells[x][y].button), " ");
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS)
                    reveal(nx, ny);
            }
    }
}

gboolean on_click(GtkWidget *w, GdkEventButton *e, gpointer data) {
    int *pos = data;
    int x = pos[0], y = pos[1];
    if (e->button == 1)
        reveal(x, y);
    else if (e->button == 3) {
        if (cells[x][y].is_revealed) return TRUE;
        cells[x][y].is_flagged = !cells[x][y].is_flagged;
        gtk_button_set_label(GTK_BUTTON(cells[x][y].button),
            cells[x][y].is_flagged ? "🚩" : " ");
    }
    return TRUE;
}

void place_mines(int skipx, int skipy) {
    int m = 0;
    while (m < MINES) {
        int x = rand() % ROWS, y = rand() % COLS;
        if ((x == skipx && y == skipy) || cells[x][y].is_mine) continue;
        cells[x][y].is_mine = TRUE;
        m++;
    }
    for (int x = 0; x < ROWS; x++)
        for (int y = 0; y < COLS; y++)
            cells[x][y].surrounding = count_mines(x, y);
}

void new_game(GtkWidget *grid) {
    srand(time(NULL));
    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)gtk_widget_destroy, NULL);
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLS; y++) {
            cells[x][y] = (Cell){0};
            GtkWidget *b = gtk_button_new_with_label(" ");
            cells[x][y].button = b;
            gtk_widget_set_hexpand(b, TRUE);
            gtk_widget_set_vexpand(b, TRUE);


	    gtk_widget_set_hexpand(b, TRUE);
            gtk_widget_set_vexpand(b, TRUE);

            int *p = malloc(sizeof(int) * 2); p[0] = x; p[1] = y;
            g_signal_connect(b, "button-press-event", G_CALLBACK(on_click), p);
            gtk_grid_attach(GTK_GRID(grid), b, y, x, 1, 1);
        }
    }
    gtk_widget_show_all(grid);
    place_mines(-1, -1);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "💣 Minesweeper");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *btn = gtk_button_new_with_label("🔁 New Game");
    g_signal_connect(btn, "clicked", G_CALLBACK(new_game), grid);

    gtk_box_pack_start(GTK_BOX(vbox), btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    new_game(grid);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
