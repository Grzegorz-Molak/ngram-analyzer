// Grzegorz Molak 310280 oświadczam że niniejsza praca została wykonana przeze mnie samodzielnie
#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;

// POCZATEK DEKLARACJI STRUKTUR ************************
int licznik = 0;
int licznik_usu = 0; // DO sprawdzenia czy tyle samo zmiennychz zostalo utworzonych i usunietych
struct n_gram // n_gram jest wezlem drzewa
{
    string value;
    n_gram* left_leave;
    n_gram* right_leave;
    int quantity;

    n_gram(string reading_frame); // konstruktor
};

struct n_gram_tree // drzewo
{
    n_gram* root;
    bool insert(string reading_frame);
    void read(n_gram* node, ofstream& result);
    void delete_mem(n_gram* node);
};

// KONIEC DEKLARACJI STRUKTUR ************************
// POCZATEK DEKLARACJI FUNKCJI ************************

int char_to_num(char sign); //zmienia char w liczbe
int create_n_length(char *argv); // zmienia argument programu w liczbe
bool arguments_quantity(int argc); // czy sa 3 argumenty do programu
string create_source_name(char *argv); // zmienia tablice charow na string
string set_RF(ifstream& to_read, unsigned int n_length); // ustawia ramke odczytu
string move_RF(ifstream& to_read, string reading_frame); // przesuwa
int read_data(string source_name, unsigned int n_length, n_gram_tree n_grams); // czyta dane
int save_data(n_gram_tree n_grams, string source_name); // zapisuje dane
int is_nucleotyd(char sign); // sprawdza czy nukleotyd to AGCTA


// KONIEC DEKLARACJI FUNKCJI ************************

int new_data = 0;
int deleted_data = 0;

int main(int argc, char **argv)
{
    int everything_ok = 1;


    if(!arguments_quantity(argc)) return 0;
    else
    {
        unsigned int n_length = create_n_length(argv[1]);
        if(n_length == 0) // Zla dlugosc n_gramu
        {
            cerr<<"Nieprawidlowo wprowadzona dlugosc n_gramu"<<endl;
            return 0;
        }

       string source_name = create_source_name(argv[2]); // nazwa pliku do odczytu

       n_gram_tree n_grams; // TWORZE DRZEWKO
       n_grams.root = new n_gram(""); // tworzy korzen
       if(!n_grams.root)
       {
           cerr<<"Wystapil blad tworzenia dynamicznych zmiennych"<<endl;
           return 0;
       }

       everything_ok = read_data(source_name, n_length, n_grams); // ODCZYTYWANIE Z PLIKU
       if(everything_ok == 0)
       {
            n_grams.delete_mem(n_grams.root);
           cerr<<"Przepraszamy, wystapil blad czytania danych"<<endl;
           return 0;
       }
       else
       {
           cout<<"Wczytano dane z pliku"<<endl;
       }

       everything_ok = save_data(n_grams, source_name); // ZAPISYWANIE W PLIKU
       if(everything_ok == 0)
       {
           n_grams.delete_mem(n_grams.root);
           cerr<<"Przepraszamy, wystapil blad zapisywania danych"<<endl;
           return 0;
       }
       else
       {
           cout<<"Zapisano dane do pliku"<<endl;
       }


      n_grams.delete_mem(n_grams.root); // USUWANIE PAMIECI
      if(new_data == deleted_data) cout<<"Usunieto dynamicznie zalokowane zmienne"<<endl;
      else
      {
          cerr<<"Nie udalo sie usunac dynamicznie zalokowanej pamieci"<<endl;
          return 0;
      }
    cout<<"Program poprawnie zakonczyl dzialanie"<<endl;
    return 1;
    }

}

// FUNKCJE ************************************************

int char_to_num(char sign)
{
    return sign - '0';
}
bool arguments_quantity(int argc)
{
    if(argc == 3)
        return 1;
    else if(argc > 3)
    {
        cerr<<"Podano za duzo argumentow do programu"<<endl;
        cerr<<"Prosze uruchomic program z poprawnymi argumentami: dlugosc n-gramu nazwa pliku fasta"<<endl;
        return 0;
    }
    else
    {
        cerr<<"Podano za malo argumentow do programu"<<endl;
        cerr<<"Prosze uruchomic program z poprawnymi argumentami: dlugosc n-gramu nazwa pliku fasta"<<endl;
        return 0;
    }
}
string create_source_name(char *argv)
{
    string source_file = "";
    for(unsigned int i = 0; i < strlen(argv); i++)
    {
        source_file += argv[i];
    }

    return source_file;
}

int create_n_length(char *argv)
{
    int result = 0;
    for(unsigned int i = 0; i < strlen(argv); i++)
    {
        int iteration_number = char_to_num(argv[i]);
        if(iteration_number >= 0 && iteration_number < 10)
        {
            result*=10; // dodaje cyfre w dobrym miejscu liczby
            result += iteration_number;
        }
        else return 0;
    }
    return result;
}

int read_data(string source_name, unsigned int n_length,  n_gram_tree n_grams)
{

    string reading_frame = "";
    bool everything_ok = true;

    ifstream to_read;
    to_read.open(source_name); // otwieram plik
    if(to_read.fail() || !to_read.is_open())
    {
        to_read.clear();
        cout<<"Nie udalo sie otworzyc pliku "<<source_name<<" "<<endl;
        return 0;
    }
    else
    {
        cout<<"Poprawnie otworzono plik: "<<source_name<<endl;
        cout<<"Trwa odczytywanie danych z pliku, * oznaczaja postep w odczytywaniu plikow"<<endl;
    }

    int pocz = to_read.tellg();
    to_read.seekg(0, ios_base::end);
    int koniec = to_read.tellg();
    to_read.seekg(0);
    koniec /= 100;
    int postep = 1;

    getline(to_read, reading_frame); // linijka z > ......

    if(reading_frame[0] != '>')
    {
        cerr<<"Pliki fasta rozpoczynaja sie od znaku: > "<<endl;
        cerr<<"Prosze przygotowac poprawny plik FASTA i uruchomic ponownie program";
        return 0;
    }

    reading_frame = set_RF(to_read, n_length); // pierwsza ramka odczytu
    (*n_grams.root).value = reading_frame;
    postep++;
   //cout<<"Pierwszy element: "<<reading_frame<<endl;

    while(!to_read.eof()) // do zakonczenia pliku
    {
        reading_frame = move_RF(to_read, reading_frame); // przesuwamy ramke
        postep++;
        if(postep == koniec)
        {
            cout<<"*";
            cout.flush();
            postep = 0;
        }
        if(reading_frame != "ERROR" && reading_frame != "END") // jezeli poprawna ramka odczytu
        {
          everything_ok = n_grams.insert(reading_frame);
          if(!everything_ok) // nie udalo sie wstawic do drzewka
          {
          cerr<<"wystapil blad czytania danych "<<endl;
          return 0;
          }
        }
    }

    cout<<endl;

    to_read.close();
    return 1;
}

string set_RF(ifstream& to_read, unsigned int n_length)
{
    string result = "";
    if(to_read.good()  && to_read.is_open())
    {
        char peek;
        while(result.length() < n_length && !to_read.eof())
        {
            peek = to_read.get();
            if(is_nucleotyd(peek) == 0)
            {
                result += peek;
            }
            else if(is_nucleotyd(peek) == 2)
            {
                cout<<"W pliku znaleziono znak spoza zbioru {AGCTN} "<<endl;
                return "ERROR";
            }
        }
        if(result.length() == n_length) return result;
        else
        {
            cerr<<"Z danych nie da sie utworzyc zadnego "<<n_length<<"-gramu"<<endl;
            return "ERROR";
        }
    }
    else
    {
        cout<<"Wystapil problem z plikiem"<<endl;
        return "ERROR";
    }
}

string move_RF(ifstream& to_read, string reading_frame)
{
    if(to_read.good() && to_read.is_open())
    {
        char peek;
        do{
            peek = to_read.get();
            if(to_read.eof())
            {
                return "END";
            }
            else if(is_nucleotyd(peek) == 2)
           {
                cerr<<"W pliku znaleziono znak spoza zbioru {AGCTN} "<<endl;
                return "ERROR";
            }
         }while(is_nucleotyd(peek) != 0); // dopoki nie znajdzie dobrego znaku to proboje pobrac znak

    reading_frame.erase(0, 1); // usuwa na poczatku
    reading_frame += peek; // dodaje na koncu
    return reading_frame;
    }
    else
    {
        cout<<"Wystapil problem z plikiem"<<endl;
        return "ERROR";
    }
}

int is_nucleotyd(char sign)
{
    if(sign == 'A' || sign == 'G' || sign == 'T' || sign == 'C')
    {
        return 0;
    }
    else if(sign == 'N' || sign == '\n')
    {
        return 1;
    }
    else
    {
        return 2; // NIEDOZWOLONY ZNAK
    }
}

int save_data(n_gram_tree n_grams, string source_name)
{
    ofstream to_save;
    to_save.open("ngrams.txt");
    if(!to_save.is_open() || to_save.bad())
    {
        to_save.clear();
        cerr<<"Wystapil blad tworzenia pliku"<<endl;
        return 0;
    }
    else if(to_save.good())
    {
        to_save<<source_name<<endl;
        n_grams.read(n_grams.root, to_save);
    }
    to_save.close();
    return 1;
}

// FUNKCJE STRUKTUR *******************************

n_gram::n_gram(string reading_frame) // KONSTRUKTOR
{
    value = reading_frame;
    left_leave = nullptr;
    right_leave = nullptr;
    quantity = 1;
}
bool n_gram_tree::insert(string reading_frame) // wstawianie danej do drzewa
{
    n_gram* pointer = root;
    while(1)
    {
        if(reading_frame > pointer->value)
        {
            if(pointer->right_leave)
            {
              pointer = pointer->right_leave;
            }
            else
            {
                pointer->right_leave = new n_gram(reading_frame);
                if(!pointer->right_leave)
                {
                    cerr<<"Blad tworzenia dynamicznych zmiennych"<<endl;
                    return false;
                }
                return true;
            }

        }

        else if(reading_frame < pointer->value)
        {
            if(pointer->left_leave)
            {
             pointer = pointer->left_leave;
            }
            else
            {
                pointer->left_leave = new n_gram(reading_frame);
                if(!pointer->left_leave)
                {
                    cerr<<"Blad tworzenia dynamicznych zmiennych"<<endl;
                    return false;
                }
                return true;
            }
        }

        else
        {
            pointer->quantity++;
            return true;
        }
    }

return false;
}

void n_gram_tree::read(n_gram* node, ofstream& result)
{

    if(node->left_leave) read(node->left_leave, result);
    result<<node->value<<" "<<node->quantity<<endl;
    new_data++;
    if(node->right_leave) read(node->right_leave, result);
}

void n_gram_tree::delete_mem(n_gram* node)
{
        if(node->right_leave) delete_mem(node->right_leave);
        if(node->left_leave) delete_mem(node->left_leave);

        delete node;
            deleted_data++;
}


