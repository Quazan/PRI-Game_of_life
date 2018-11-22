#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int x = 80;
int y = 25;
int i, j;
int aktualna[100][100];
int pokolenia = 100;
int alive = 0;
int zapis = 5;
int czas = 150000;
char plik[100];
FILE *fp;

void ustawienia(int stan);
void czyszczenie();
void wypisz();
void kontynuacja();
void wlasne();
void losowe();
int  menu_stop(int poko);
void gra(int poko);
void start();
void wczytaj();
int dane_plik();
int dane();
void zapisywanie();

int usleep(useconds_t usec);

int main()
{
    srand(time(NULL));

    start();

    return 0;
}

void zapisywanie(int poko)
{
    fprintf(fp, "Nr. pokolenia: %d\n<br>\n", pokolenia - poko);
    fprintf(fp, "<br>\n<table border=\"3\">\n<tr>\n<td>\n");

    for(i = 1; i <= y; i++)
    {
        for(j = 1; j < x; j++)
        {
            if(aktualna[j][i] == 1)
            {
                fprintf(fp, "#");
            }
            else
            {
                fprintf(fp, "<font color=\"white\">Q</font>");
            }
        }
        if(aktualna[j][i] == 1)
            {
                fprintf(fp, "#");
            }
            else
            {
                fprintf(fp, "<font color=\"white\">Q</font>");
            }
        fprintf(fp, "<br>\n");
    }
    fprintf(fp, "</td>\n</tr>\n</table>\n<br>\n<br>\n");
}

int dane()
{
    char znak;
    int liczba = 0;

    while((znak = getch()) != 10)
    {
        if(znak < 48 || znak > 57)
        {
            return 0;
        }

        liczba *= 10;
        liczba += (znak - 48);
    }
    return liczba;
}

int dane_plik()
{
    char znak;
    int liczba = 0;

    while((znak = fgetc(fp)) != 10)
    {
        if(znak == 32) continue;
        if(znak < 48 && znak > 57)
        {
            return 0;
        }
        liczba *= 10;
        liczba += (znak - 48);
    }
    return liczba;
}

void czyszczenie()
{
    alive = 0;

    for(j = 1; j <= y; j++)
    {
        for(i =1; i <= x; i++)
        {
            aktualna[i][j] = 0;
        }
    }
}

void kontynuacja()
{
    const char txt1[] = "Nowa gra";
    const char txt2[] = "Wczytaj ostatni stan";

    int znak;
    int ktory = 1;

    const short int min_wybor = 1;
    const short int max_wybor = 2;

    wypisz();
    mvprintw( 6, 5, txt2 );
    attron(A_REVERSE);
    mvprintw( 5, 5, txt1 );
    attroff(A_REVERSE);

    while(znak != 10)
    {
        znak = getch();

        if(znak == KEY_UP && ktory != min_wybor) ktory--;
        else if(znak == KEY_DOWN && ktory != max_wybor) ktory++;

        wypisz();

        switch(ktory)
        {
            case 1:
                mvprintw( 6, 5, txt2 );
                attron(A_REVERSE);
                mvprintw( 5, 5, txt1 );
                break;
               
            case 2:
                mvprintw( 5, 5, txt1 );
                attron(A_REVERSE);
                mvprintw( 6, 5, txt2 );
                break;
        }
        attroff(A_REVERSE);
    }

    clear();

    switch(ktory)
    {
            case 1:
                czyszczenie();
                ustawienia(2);
                break;

            case 2:
                ustawienia(2);
                break;
    }
}

void wypisz()
{
    clear();

    mvhline( 0,  51, ACS_HLINE, x);
    mvhline( y+1, 51, ACS_HLINE, x);
    mvvline( 1, 50, ACS_VLINE, y);
    mvvline( 1, x+51, ACS_VLINE, y);
    mvaddch(0, 50, ACS_ULCORNER);
    mvaddch(y+1, 50, ACS_LLCORNER);
    mvaddch(0, x+51, ACS_URCORNER);
    mvaddch(y+1, x+51, ACS_LRCORNER);


    for(j = 1; j <= y; j++)
    {
        for(i =1; i <= x; i++)
        {   
            attron(A_BOLD);
            if(aktualna[i][j] == 1) mvaddch( j, i + 50, ACS_BULLET);
            else mvprintw( j, i + 50, " ");
            attroff(A_BOLD);
        }
    }
}

void wlasne()
{
    int akt_x = 0;
    int akt_y = 0;
    int znak;

    const short int min_x = 50;
    const short int max_x = x+50-1;
    const short int min_y = 1;
    const short int max_y = y;

    akt_x = (min_x+max_x+1)/2;
    akt_y = (min_y+max_y+1)/2;

    wypisz();
    mvprintw(1, 1, "Uzyj kursorow do poruszania sie po planszy");
    mvprintw(2, 1, "Uzyj spacji do zmiany stanu danego punktu");
    mvprintw(3, 1, "Kliknij ENTER aby rozpoczac");

    while( znak != 10 )
    {

        znak = getch();

        if(znak == KEY_UP && akt_y != min_y) akt_y--;
        else if(znak == KEY_DOWN && akt_y != max_y) akt_y++;
        else if(znak == KEY_LEFT && akt_x != min_x) akt_x--;
        else if(znak == KEY_RIGHT && akt_x != max_x) akt_x++;
        else if(znak == KEY_UP && akt_y == min_y) akt_y = max_y;
        else if(znak == KEY_DOWN && akt_y == max_y) akt_y = min_y;
        else if(znak == KEY_LEFT && akt_x == min_x) akt_x = max_x;
        else if(znak == KEY_RIGHT && akt_x == max_x) akt_x = min_x;
        else if(znak == 32)
        {
            if(aktualna[akt_x-50+1][akt_y] == 1) 
            {
                aktualna[akt_x-50+1][akt_y] = 0;
                alive--;
            }
            else 
            {
                aktualna[akt_x-50+1][akt_y] = 1;
                alive++;
            }
        }

        wypisz();
        mvprintw(1, 1, "Uzyj kursorow do poruszania sie po planszy");
        mvprintw(2, 1, "Uzyj spacji do zmiany stanu danego punktu");
        mvprintw(3, 1, "Kliknij ENTER aby rozpoczac");

        attron( A_REVERSE );
        attron(A_BOLD);
        if(aktualna[akt_x-50+1][akt_y] == 1) mvaddch( akt_y, akt_x, ACS_BULLET);
        else mvprintw(akt_y, akt_x, " ");
        attroff( A_REVERSE );

        if(akt_x-50 == 0) mvaddch( akt_y, akt_x, ACS_VLINE);
        else
        {
            if(aktualna[akt_x-50][akt_y] == 1) mvaddch( akt_y, akt_x, ACS_BULLET);
            else mvprintw(akt_y, akt_x, " ");
        }
        attroff(A_BOLD);
    }
}

void losowe()
{
    for(i =1; i <= x; i++)
    {
        for(j = 1; j <= y; j++)
        {
            aktualna[i][j] = rand()%2;
            if(aktualna[i][j] == 1) alive++;
        }
    }
}

int menu_stop(int poko)
{
    const char txt1[] = "Kontynuuj";
    const char txt2[] = "Zmiana punktow";
    const char txt3[] = "Wyjscie";

    int ktory = 1;
    int znak;

    const short int min_wybor = 1;
    const short int max_wybor = 3;

    while(ktory != 3 || znak != 10)
    {
        wypisz();

        switch(ktory)
        {
        case 1:
            mvprintw( 6, 5, txt2 );
            mvprintw( 7, 5, txt3 );
            attron(A_REVERSE);
            mvprintw( 5, 5, txt1 );
            break;
           
        case 2:
            mvprintw( 5, 5, txt1 );
            mvprintw( 7, 5, txt3 );
            attron(A_REVERSE);
            mvprintw( 6, 5, txt2 );
            break;
           
        case 3:
            mvprintw( 5, 5, txt1 );
            mvprintw( 6, 5, txt2 );
            attron(A_REVERSE);
            mvprintw( 7, 5, txt3 );
            break;
        }
        attroff( A_REVERSE );

        znak = getch();

        if(znak == KEY_UP && ktory != min_wybor) ktory--;
        else if(znak == KEY_DOWN && ktory != max_wybor) ktory++;
         
        if(znak == 10 && ktory != 3)
        {
            switch (ktory)
            {
                case 1:
                    return 1;
                    break;

                case 2:
                    if((pokolenia-poko)%zapis != 0)
                    {
                        zapisywanie(poko);
                    }
                    wlasne();
                    fprintf(fp, "EDYCJA PUNKTOW\n");
                    if((pokolenia-poko)%zapis != 0)
                    {
                        zapisywanie(poko);
                    }

                    break;
            }
        }

        wypisz();
    }
    return 2;
}

void gra(int poko)
{
	clear();

	int bufor[100][100];
	int sprawdza = 0;
    int wartownik = 0;

    echo();
    printw("Wpisz nazwe pliku do zapisu\n");
    scanw("%s", plik);
    clear();

    fp = fopen(plik, "w");

    fprintf(fp, "<html>\n<head>\n <title>%s</title>\n</head>\n<body>\n<center>\n<h1>Game of life</h1>\n<br>\n<h2>Wyniki symulacji</h2>\n<br>\n", plik);
    clear();
    noecho();

    timeout(0);
	while(poko > 0 && !sprawdza && alive != 0)
	{	
        if((pokolenia-poko)%zapis == 0)
        {
            zapisywanie(poko);
        }
        usleep(1);

        sprawdza = getch();

        if(sprawdza == 32)
        {
            timeout(1000000);
            wartownik = menu_stop(poko);
            timeout(0);

            if(wartownik == 1) sprawdza = 0;
            else sprawdza  = 1;
        }
        else
        {
            sprawdza = 0;
            
    		for(i =1; i <= x; i++)
    		{
    			aktualna[i][0] = aktualna[i][y];
    			aktualna[i][y+1] = aktualna[i][1];
    		}
    		for(i =1; i <= y; i++)
    		{
    			aktualna[0][i] = aktualna[x][i];
    			aktualna[x+1][i] = aktualna[1][i];
    		}

    		aktualna[0][0] = aktualna[x][y];
    		aktualna[0][y+1] = aktualna[x][1];
    		aktualna[x+1][0] = aktualna[1][y];
    		aktualna[x+1][y+1] = aktualna[1][1];
            
            wypisz();
            mvprintw( 5, 6, "Wcisnij spacje zeby zatrzymac");
            attron(A_STANDOUT);
            mvprintw( 1, 1, "Numer pokolenia \t%d", pokolenia-poko);
            attroff(A_STANDOUT);

    		for(i =1; i <= x; i++)
    		{
    			for(j = 1; j <= y; j++)
    			{
    				bufor[i][j] = aktualna[i+1][j] + aktualna[i-1][j] + aktualna[i][j+1] + aktualna[i][j-1] + aktualna[i+1][j+1] + aktualna[i-1][j-1] + aktualna[i+1][j-1] + aktualna[i-1][j+1];
    			}
    		}

    		for(i =1; i <= x; i++)
    		{
    			for(j = 1; j <= y; j++)
    			{
    				if(aktualna[i][j] == 1 && (bufor[i][j] != 2 && bufor[i][j] != 3))
                    {
                        aktualna[i][j] = 0;
                        alive--;
                    }
    				else if(aktualna[i][j] == 0 && bufor[i][j] == 3) 
                    {
                        aktualna[i][j] = 1;	
                        alive++;
                    }
    			}
    		}

    		poko--;

    		refresh();
    		fflush(stdout);
    		usleep(czas);
        }
	}
    zapisywanie(poko);
    fprintf(fp, "</center>\n</body>\n</html> ");
    fclose(fp);
    timeout(1000000);
    clear();
}

void ustawienia(int stan)
{
    const char txt0[] = "Start";
	const char txt1[] = "Szerokosc planszy";
    const char txt2[] = "Wysokosc planszy";
    const char txt3[] = "Czas pomiedzy iteracjami";
    const char txt4[] = "Ilosc pokolen"; 
    const char txt5[] = "Zapis stanu";
    const char txt6[] = "Wyjscie";

    int ktory = 0;
    int znak;

    const short int min_wybor = 0;
    const short int max_wybor = 6;


    mvprintw( 10, 8, txt1 );
    mvprintw( 11, 8, txt2 );
    mvprintw( 12, 8, txt3 );
    mvprintw( 13, 8, txt4 );
    mvprintw( 14, 8, txt5 );
    mvprintw( 15, 8, txt6 );
    attron(A_REVERSE);
    mvprintw( 9, 8, txt0 );
    attroff(A_REVERSE);

    mvprintw( 1, 1, "Szerokosc \t %d\n", x);
    mvprintw( 2, 1, "Wysokosc \t %d\n", y);
    mvprintw( 3, 1, "Opoznienie \t %d mikrosekund\n", czas);
    mvprintw( 4, 1, "Pokolenia \t %d\n", pokolenia);
    mvprintw( 5, 1, "Zapisywanie \t %d pokolen\n", zapis);

    while(ktory != 6 || znak != 10)
    {
        znak = getch();

        clear();

        if(znak == KEY_UP && ktory != min_wybor) ktory--;
        else if(znak == KEY_DOWN && ktory != max_wybor) ktory++;
         
        if(znak == 10)
        {
            switch(ktory)
            {
            case 0:
            	switch(stan)
            	{
            		case 1:
            			losowe();
                        break;

            		case 2:
            			wlasne();
                        break;
            	}

                gra(pokolenia);

                return;
                break;

            case 1:
            	printw("Wpisz szerokosc planszy z zakresu 3 - 80\n");
                echo();
                x = dane();
            	while(x < 3 || x > 80)
            	{
                    clear();
            		printw("Blad danych, wpisz jeszcze raz\n");
                    x = dane();
            	}

                noecho();
                break;

            case 2:
            	printw("Wpisz wysokosc planszy z zakresu 3 - 25\n");
                echo();
                y = dane();
            	while(y < 3 || y > 25)
            	{
            		clear();
            		printw("Blad danych, wpisz jeszcze raz\n");
                    y = dane();
            	}

                noecho();
                break;

            case 3:
            	printw("Wpisz dlugosc opoznienia pomiedzy kolejnymi iteracjami w mikrosekundach (1 sekunda - 1 000 000 mikrosekund)\n");
                echo();
                czas = dane();
            	while(czas <= 0 || czas > 10000000)
            	{
            		clear();
            		printw("Blad danych, wpisz jeszcze raz\n");
                    czas = dane();
            	}

                noecho();
                break;

            case 4:
            	printw("Wpisz ilosc pokolen z zakresu 1 - 1000000\n");
                echo();
                pokolenia = dane();
            	while(pokolenia <= 0 || pokolenia > 1000000)
            	{
            		clear();
            		printw("Blad danych, wpisz jeszcze raz\n");
                    pokolenia = dane();
            	}

                noecho();
                break;

                case 5:
                printw("Wpisz co ile pokolen ma byc zapisywany stan symulacji do pliku (nie wiecej niz %d)\n", pokolenia);
                echo();
                zapis = dane();
                while(zapis <= 0 || zapis > pokolenia)
                {
                    clear();
                    printw("Blad danych, wpisz jeszcze raz\n");
                    zapis = dane();
                }

                noecho();
                break;
            }
        }
        
        clear();

        mvprintw( 1, 1, "Szerokosc \t %d\n", x);
    	mvprintw( 2, 1, "Wysokosc \t %d\n", y);
    	mvprintw( 3, 1, "Opoznienie \t %d mikrosekund\n", czas);
    	mvprintw( 4, 1, "Pokolenia \t %d\n", pokolenia);
        mvprintw( 5, 1, "Zapisywanie \t %d pokolen\n", zapis);
    	
        switch(ktory)
        {
        case 0:
        	mvprintw( 10, 8, txt1 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 14, 8, txt5 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 9, 8, txt0 );
            break;

        case 1:
        	mvprintw( 9, 8, txt0 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 14, 8, txt5 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 10, 8, txt1 );
            break;
           
        case 2:
        	mvprintw( 9, 8, txt0 );
            mvprintw( 10, 8, txt1 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 14, 8, txt5 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 11, 8, txt2 );
            break;
           
        case 3:
        	mvprintw( 9, 8, txt0 );
            mvprintw( 10, 8, txt1 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 14, 8, txt5 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 12, 8, txt3 );
            break;

        case 4:
        	mvprintw( 9, 8, txt0 );
            mvprintw( 10, 8, txt1 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 14, 8, txt5 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 13, 8, txt4 );
            break;

        case 5:
        	mvprintw( 9, 8, txt0 );
            mvprintw( 10, 8, txt1 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 15, 8, txt6 );
            attron(A_REVERSE);
            mvprintw( 14, 8, txt5 );
            break;

        case 6:
            mvprintw( 9, 8, txt0 );
            mvprintw( 10, 8, txt1 );
            mvprintw( 11, 8, txt2 );
            mvprintw( 12, 8, txt3 );
            mvprintw( 13, 8, txt4 );
            mvprintw( 14, 8, txt5 );
            attron(A_REVERSE);
            mvprintw( 15, 8, txt6 );
            break;  
        }
        attroff(A_REVERSE);
    }

    clear();
}

void start()
{
	initscr();

    noecho();
    keypad(stdscr, TRUE);

    const char txt0[] = "Wczytaj z pliku";
    const char txt1[] = "Losowy wybor punktow";
    const char txt2[] = "Wybor wlasnych punktow";
    const char txt3[] = "Wyjscie";

    int ktory = 0;
    int znak;

    const short int min_wybor = 0;
    const short int max_wybor = 3;

    attron(A_STANDOUT);
    attron(A_BOLD);
    mvprintw(2, 11, "GAME OF LIFE");
    attroff(A_STANDOUT);
    attroff(A_BOLD);

    mvprintw( 5, 5, txt1 );
    mvprintw( 6, 5, txt2 );
    mvprintw( 7, 5, txt3 );
    attron(A_REVERSE);
    mvprintw( 4, 5, txt0 );
    attroff(A_REVERSE);

    while( ktory != 3 || znak != 10 )
    {  
        znak = getch();

        clear();

        if(znak == KEY_UP && ktory != min_wybor) ktory--;
        else if( znak == KEY_DOWN && ktory != max_wybor ) ktory++;
        if(znak == 10 && ktory != 3)
        {
            if(alive && ktory == 2) kontynuacja();
            else if(ktory == 0)
            {
                czyszczenie();
                wczytaj();
            }
        	else 
            {
                czyszczenie();
                ustawienia(ktory);
            }
        }

        if(alive) wypisz();

        attron(A_STANDOUT);
        attron(A_BOLD);
        mvprintw(2, 11, "GAME OF LIFE");
        attroff(A_STANDOUT);
        attroff(A_BOLD);

        switch(ktory)
        {
        case 0:
            mvprintw( 5, 5, txt1 );
            mvprintw( 6, 5, txt2 );
            mvprintw( 7, 5, txt3 );
            attron(A_REVERSE);
            mvprintw( 4, 5, txt0 );
            break;

        case 1:
            mvprintw( 4, 5, txt0 );
            mvprintw( 6, 5, txt2 );
            mvprintw( 7, 5, txt3 );
            attron(A_REVERSE);
            mvprintw( 5, 5, txt1 );
            break;
           
        case 2:
            mvprintw( 4, 5, txt0 );
            mvprintw( 5, 5, txt1 );
            mvprintw( 7, 5, txt3 );
            attron(A_REVERSE);
            mvprintw( 6, 5, txt2 );
            break;
           
        case 3:
            mvprintw( 4, 5, txt0 );
            mvprintw( 5, 5, txt1 );
            mvprintw( 6, 5, txt2 );
            attron(A_REVERSE);
            mvprintw( 7, 5, txt3 );
            break;
        }
        attroff(A_REVERSE);
    }
   
    clear();    
    mvprintw(0, 0, "Koniec programu, przycisnij przycisk..." );
    znak = getch();
    endwin();
}

void wczytaj()
{
    echo();
    printw("Wpisz nazwe pliku z danymi\n");
    scanw("%s", plik);
    if(plik[0] == 27)
    {
        clear();
        return;
    }
    fp = fopen(plik, "r");
    while(fp == NULL)
    {
        clear();
        printw("Wpisz ponownie nazwe pliku\n");
        scanw("%s", plik);
        if(plik[0] == 27)
        {
            clear();
            return;
        }
        fp = fopen(plik, "r");
    }
    char znak;

    znak = fgetc(fp);
    if(znak != 's')
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        znak = getch();
        return;
    }

    x = dane_plik();

    while(x < 3 || x > 80)
    {
        clear();
        printw(" BLAD\n Wpisz szerokosc planszy z zakresu 3 - 80\n ");
        x = dane();
        clear();
    }

    znak = fgetc(fp);
    if(znak != 'w')
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        znak = getch();
        clear();
        return;
    }

    y = dane_plik();

    while(y < 3 || y > 25)
    {
        clear();
        printw(" BLAD\n Wpisz wysokosc planszy z zakresu 3 - 25\n ");
        y = dane();
        clear();
    }

    znak = fgetc(fp);
    if(znak != 'o')
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        znak = getch();
        clear();
        return;
    }

    czas = dane_plik();

    while(czas <= 0 || czas > 100000000)
    {
        clear();
        printw(" BLAD\n Wpisz dlugosc opoznienia pomiedzy kolejnymi iteracjami w mikrosekundach (1 sekunda - 1 000 000 mikrosekund)\n ");
        czas = dane();
        clear();
    }

    znak = fgetc(fp);
    if(znak != 'i')
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        znak = getch();
        return;
    }

    pokolenia = dane_plik();

    while(pokolenia <= 0 || pokolenia > 1000000)
    {
        clear();
        printw(" BLAD\n Wpisz ilosc pokolen z zakresu 1 - 1000000\n ");
        pokolenia = dane();
        clear();
    }

    znak = fgetc(fp);
    if(znak != 'n')
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        znak = getch();
        return;
    }

    zapis = dane_plik();

    while(zapis <= 0 || zapis > pokolenia)
    {
        clear();
        printw(" BLAD\n Wpisz co ile pokolen ma byc zapisywany stan symulacji do pliku (nie wiecej niz %d)\n ", pokolenia);
        zapis = dane();
        clear();
    }

    for(i = 1; i <= y; i++)
    {
        znak = fgetc(fp);
        if(znak != 10)
        {
            clear();
            printw("Wcisnij dowolny przycisk");
            mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
            czyszczenie();
            znak = getch();
            clear();
            return;

        }
        for(j = 1; j <= x; j++)
        {
            znak = fgetc(fp);

            if(znak - 48 == 0)
            {
                aktualna[j][i] = 0;
            }
            else if(znak - 48 == 1)
            {
                aktualna[j][i] = 1;
                alive++;
            }
            else 
            {
                clear();
                printw("Wcisnij dowolny przycisk");
                mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
                czyszczenie();
                znak = getch();
                clear();
                return;
            }
        }
    }
    znak = fgetc(fp);
    if(znak != -1)
    {
        clear();
        printw("Wcisnij dowolny przycisk");
        mvprintw(5, 5, "BLAD DANYCH WEJSCIOWYCH\n");
        czyszczenie();
        znak = getch();
        clear();
        return;
    }
    fclose(fp);
    noecho();
    gra(pokolenia);
}