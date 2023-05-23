// Grzegorz Molak 310280 oświadczam że niniejsza praca została wykonana przeze mnie samodzielnie
#include <iostream>
#include <string.h>
#include <fstream>
#include <iomanip>


using namespace std;

//*********** STRUKTURY *********

struct n_gram // pojedynczy ngram
{
    string text;
    n_gram* l_leave;
    n_gram* r_leave;
    int quantity;

    n_gram(string text_f, int quantity_f);
};

struct n_gram_tree // drzewo ngramow
{
    n_gram* root;
    int elements;
    string source;
    void find_text(n_gram* point, string wanted, bool &found);
    void show_range(int min, int max, n_gram* point);
    void read(n_gram* point);
    void insert(n_gram read);
    void deletemem(n_gram* point);

    n_gram_tree();
};

//*********** STRUKTURY *********
//*********** FUNKCJE *********

void read_file(n_gram_tree &tree); //odczyt ngrams.txt
bool is_n_gram(string ngram); // sprawdza czy tekst to ngram
void find_n_gram(n_gram_tree tree, unsigned int n_length); //szukanie ngramu
bool show_range(n_gram_tree tree); // w zakresie
int pow(int basis, int exp, int result = 1); // potegowanie
void options(); // wyswietla menu

//*********** FUNKCJE *********
// PROGRAM ***************************************************
int main()
{
    char tryb; // opcja menu
    n_gram_tree tree; // drzewo


    read_file(tree);

    unsigned int n_length = tree.root->text.length();
    long long max = pow(4, n_length);
    cout<<"Odczytano "<<n_length<<"-gramy stworzone z pliku FASTA: "<<tree.source<<endl;
    cout<<"Utworzono "<<tree.elements<<" z "<<max<<" mozliwych "<<n_length<<"-gramow"<<endl;
    cout<<"To "<<setprecision(3)<<double(100.00 * tree.elements/ max)<<"% mozliwych "<<n_length<<"-gramow"<<endl;

    do
    {
        options();
        cin>>tryb;
        if(cin.fail())
        {
            cout<<"Wprowadzono nieprawidlowo wartosc"<<endl;
            cin.clear();
            cin.ignore();
            continue;
        }
        switch(tryb)
        {
        case '0':
            cout<<"Koncze dzialanie programu"<<endl;
            break;
        case '1':
            find_n_gram(tree,n_length);
            break;
        case '2':
            show_range(tree);
            break;
        case '3':
            tree.read(tree.root);
            break;
        default:
            cout<<"Wybrano niepoprawna opcje"<<endl;
            getline(cin,tree.source);
            break;
        }

    }while(tryb != '0');


    tree.deletemem(tree.root); // usuwanie pamieci
    return 0;
}
// PROGRAM ***************************************************
// ********** FUNKCJE **************

int pow(int basis, int exp, int result)
{
    if(exp > 0) return pow(basis, exp-1, result*basis);
    else return result;
}

void read_file(n_gram_tree &tree)
{
    n_gram read("",0); // struktura do przechowywania odczytanych danych chwilowo
    ifstream to_read; // strumien
    to_read.open("ngrams.txt");
    if(to_read.bad() || !to_read.is_open())
    {
        cout<<"Nie udalo sie otworzyc pliku: ngrams.txt "<<endl;
    }
    else
    {
            cout<<"Otworzono plik ngrams.txt"<<endl<<"Trwa wczytywanie ngramow"<<endl;
            getline(to_read,tree.source); // w pierwszej linijce jest zrodlo ngramow
            to_read>>read.text>>read.quantity; // pobieram dane do root
           // cout<<read.quantity;
            if(to_read.fail())
            {
                cout<<"blad wczytywania"<<endl;
                return;
            }
            tree.root = new n_gram(read.text, read.quantity); // nowy root
            tree.elements++;
            if(!tree.root) cout<<"Blad tworzenia drzewa";

        while(1)
        {
            to_read>>read.text>>read.quantity; // wczytywanie ngramu
            if(to_read.eof())
            {
                break;
            }
            tree.insert(read); // wstawianie ngramu do drzewa
        }

       // tree.read(tree.root);
    }

}

bool is_n_gram(string ngram)
{
    for(unsigned int i = 0; i < ngram.length(); i++)
    {
        if(ngram[i] != 'A' && ngram[i] != 'G' && ngram[i] != 'C' && ngram[i] != 'T') // ZNAK SPOZA AGCT
        {
            return false;
        }
    }
    return true;
}


void find_n_gram(n_gram_tree tree,unsigned int n_length)
{
    bool found = false;
    string wanted;

    cout<<"Prosze wpisac poszukiwany ngram"<<endl;

    cin>>wanted;

    if(is_n_gram(wanted) && wanted.length() == n_length) // DOBRY NGRAM
    {
        tree.find_text(tree.root, wanted, found);
        if(!found)
        {
            cout<<"Nie udalo sie znalezc danego ngramu, czestosc "<<wanted<<" to 0"<<endl;
        }
    }
    else
    {
        if(!is_n_gram(wanted))
        {
           cout<<"Wprowadzona wartosc nie jest n_gramem"<<endl;
        }
        else
        {
            cout<<"Analizowane sa "<<n_length<<"_gramy, a nie "<<wanted.length()<<"_gramy"<<endl;
        }
    }
}

bool show_range(n_gram_tree tree)
{
    int min;
    int max;

    cout<<"Podaj dolna wartosc zakresu"<<endl;
    cin>>min;
    if(cin.fail())
    {
        cout<<"Podano zla wartosc"<<endl;
        return false;
    }

    cout<<"Podaj gorna wartosc zakresu"<<endl;
    cin>>max;
    if(cin.fail())
    {
        cout<<"Podano zla wartosc"<<endl;
        return false;
    }

    if(min <= max)
    {
        cout<<"ngramy z danego zakresu: "<<endl;
        tree.show_range(min, max, tree.root); // WYSZUKIWANIE ZAKRESU
        return true;
    }
    else
    {
        cout<<"Dolna wartosc zakresu nie moze byc wieksza od gornego"<<endl;
        return false;
    }

}

void options()
{
    cout<<"Aby:"<<endl;
    cout<<"Wyszukac ngram wraz z jego czestoscia wpisz 1"<<endl;
    cout<<"Wyswietlic ngramy o czestosci z zakresu <A,B> wpisz 2"<<endl;
    cout<<"Wyswietlic cala baze ngramow posortowanej wg czestosci wpisz 3"<<endl;
    cout<<"Zakonczyc wpisz 0"<<endl;

}


//*********** FUNKCJE STRUKTUR *********

    //*********** KONSTRUKTORY *********
n_gram::n_gram(string text_f, int quantity_f)
{
    text = text_f;
    quantity = quantity_f;
    l_leave = nullptr;
    r_leave = nullptr;
}

n_gram_tree::n_gram_tree()
{
    root = nullptr;
    elements = 0;
    source = "";
}
    //*********** KONSTRUKTORY *********

void n_gram_tree::insert(n_gram read)
{
    n_gram* point = root;
    n_gram* point_previous = nullptr;
    while(point != nullptr) // SZUKANIE ODPOWIEDNIEGO MIEJSCA DO WSTAWIENIA WARTOSCI
    {
        point_previous = point;
        if(read.quantity > point->quantity)
        {
            point = point->r_leave;
        }
        else
        {
            point = point->l_leave;
        }

    }

    if(read.quantity > point_previous->quantity) // WIEKSZE TO W PRAWO
    {
        point_previous->r_leave = new n_gram(read.text, read.quantity);
        elements++;
        if(!point_previous->r_leave) cout<<"Blad tworzenia galezi drzewa"<<endl;
    }
    else // MNIEJSZE LUB ROWNE TO W LEWO
    {
        point_previous->l_leave = new n_gram(read.text,read.quantity);
        elements++;
        if(!point_previous->l_leave) cout<<"Blad tworzenia galezi drzewa"<<endl;
    }


}

void n_gram_tree::find_text(n_gram* point, string wanted, bool &found)
{
        if(point->text == wanted )
        {
            found = true;
            cout<<wanted<<" zostal odnaleziony. Jego czestosc: "<<point->quantity<<endl;
        }
        else
        {
            if(point->l_leave)  find_text(point->l_leave, wanted, found);
             //cout<<point->text<<" "<<point->quantity<<endl;
            if(point->r_leave)  find_text(point->r_leave, wanted, found);
        } // PRZECHODZENIE DRZEWA Z WYPISANIEM TYLKO ODPOWIEDNIEGO ELEMENTU

}

void n_gram_tree::show_range(int min, int max, n_gram *point)
{
    if(point->l_leave) show_range(min, max, point->l_leave);

    if(point->quantity >= min && point->quantity <= max)
    {
        cout<<point->text<<" "<<point->quantity<<endl;
    }

    if(point->r_leave) show_range(min, max,point->r_leave);
} // REKURENCYJNA

void n_gram_tree::read(n_gram* point)
{
    if(point->l_leave) read(point->l_leave);
    cout<<point->text<<" "<<point->quantity<<endl;
    if(point->r_leave) read(point->r_leave);

} // REKURENCYJNA

void n_gram_tree::deletemem(n_gram *point)
{
    if(point->l_leave) deletemem(point->l_leave);
    if(point->r_leave) deletemem(point->r_leave);

    if(point)
    {
        delete point;
    }


} // REKURENCYJNA
