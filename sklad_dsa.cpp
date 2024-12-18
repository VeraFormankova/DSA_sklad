﻿#include <locale>
#include <string>
#include <ctime>
#include <iostream>
#include <limits>


using namespace std;

// Struktura pro ukladani informaci o zbozi
class Record {
public:
    int id;
    string title;
    unsigned int ks;
    double price;
    string category;
    time_t datum_posledniho_naskladneni;
    Record* left;
    Record* right;

    Record(int id, string title, unsigned int ks, double price, string category)
        : id(id), title(title), ks(ks), price(price), category(category), left(nullptr), right(nullptr) {
        time(&datum_posledniho_naskladneni);
    }
};

class Sklad {
private:
    Record* root;

    // Vlozi novou polozku na sklad
    // Pokud je node nullptr, vytvori novy uzel
    // Pokud je id mensi nez id nodu, vlozi se do leveho podstromu
    // Pokud je id vetsi nez id nodu, vlozi se do praveho podstromu
    // Pokud je id rovno id nodu, aktualizuje se pocet kusu a datum posledniho naskladneni
    Record* vloz(Record* node, int id, string title, unsigned int ks, double price, string category) {
        if (node == nullptr) {
            return new Record(id, title, ks, price, category);
        }
        if (id < node->id) {
            node->left = vloz(node->left, id, title, ks, price, category);
        }
        else if (id > node->id) {
            node->right = vloz(node->right, id, title, ks, price, category);
        }
        else {
            node->ks += ks; // Aktualizace počtu kusů
            time(&node->datum_posledniho_naskladneni); // Aktualizace data
        }
        return node;
    }

    // Vypise obsah skladu
    // Pouziva inorder pruchod stromem - Leva - Root - Prava
    void vypis(Record* node) {
        if (node != nullptr) {
            // Prochazeni leveho podstromu
            vypis(node->left);

            char* buffer = ctime(&node->datum_posledniho_naskladneni);

            // Vypis informaci o zbozi - koren
            cout << "ID: " << node->id << ", Název: " << node->title << ", Kategorie: " << node->category
                << ", Počet kusů: " << node->ks << ", Cena: " << node->price
                << ", Datum naskladnění: " << buffer;

            // Prochazeni praveho podstromu
            vypis(node->right);
        }
    }

    // Vypise obsah skladu podle kategorie
    // Pouziva inorder pruchod stromem - Leva - Root - Prava
    void vyhledat_kategorii(Record* node, const string& category) {
        if (node != nullptr) {
            vyhledat_kategorii(node->left, category);
            if (node->category == category) {
                char* buffer = ctime(&node->datum_posledniho_naskladneni);

                cout << "ID: " << node->id << ", Název: " << node->title << ", Počet kusů: " << node->ks
                    << ", Cena: " << node->price << ", Datum naskladnění: " << buffer;
            }
            vyhledat_kategorii(node->right, category);
        }
    }

    // Vyhleda polozku podle ID
    // Pokud node je nullptr nebo node->id je rovno hledanemu ID, vrati node
    Record* vyhledat_id(Record* node, int id) {
        if (node == nullptr || node->id == id)
            return node;

        // Pokud je id mensi nez id nodu, hleda v levem podstromu
        if (id < node->id)
            return vyhledat_id(node->left, id);
        // Pokud je id vetsi nez id nodu, hleda v pravem podstromu
        return vyhledat_id(node->right, id);
    }

    // Odstrani polozku podle ID
    Record* odstranit(Record* node, int id) {
        // Pokud je node nullptr, vrati nullptr, to se stane pokud je uzel prazdny
        if (node == nullptr) return nullptr;

        // hledani polozky v levem podstromu. Pokud je id mensi nez id nodu, hleda v levem podstromu
        if (id < node->id) {
            node->left = odstranit(node->left, id);
        }
        // hledani polozky v pravem podstromu. Pokud je id vetsi nez id nodu, hleda v pravem podstromu
        else if (id > node->id) {
            node->right = odstranit(node->right, id);
        }
        // Pokud je id rovno id nodu, odstrani uzel
        else {
            // pokud levy nema potomka, vytvori se docasny uzel a priradi se pravy potomek
            // nalezeny se smaze
            if (node->left == nullptr) {
                Record* temp = node->right;
                delete node;
                return temp;
            }
            // pokud pravy nema potomka, vytvori se docasny uzel a priradi se levy potomek
            // nalezeny se smaze
            else if (node->right == nullptr) {
                Record* temp = node->left;
                delete node;
                return temp;
            }
            // pokud uzel ma dva potomky, najde se nejmensi uzel v pravem podstromu
            // tim se zajisti, ze se zachova struktura binarniho stromu
            // vsechno v levem podstromu je mensi nez nejnižší uzel v pravem podstromu
            else {
                Record* temp = min_value_node(node->right);
                node->id = temp->id;
                node->title = temp->title;
                node->ks = temp->ks;
                node->price = temp->price;
                node->category = temp->category;
                node->datum_posledniho_naskladneni = temp->datum_posledniho_naskladneni;
                node->right = odstranit(node->right, temp->id);
            }
        }
        return node;
    }

    // pomocna funkce pro nalezeni nejmensiho uzlu
    Record* min_value_node(Record* node) {
        Record* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    // template pro kontrolu vstupu od uzivatele
    // pouze pro numerické hodnoty
    template <typename T>
    void check_numeric_input(T& input, const string& prompt) {
        while (true) {
            cout << prompt;
            cin >> input;

            if (cin.fail()) {
                // Vyčištění vstupního bufferu před čtením řetězce
                cin.clear();
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
                cout << "Neplatný vstup. Zadejte správný datový typ\n";
            } else {
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
        }
    }

    // template pro kontrolu vstupu od uzivatele
    // pouze pro řetězce
    void check_string_input(string& input, const string& prompt) {
    while (true) {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "Neplatný vstup. Zadejte neprázdný řetězec\n";
        } else {
            break;
        }
    }
}

public:
    // inicializace skladu s prazdnym korenem
    Sklad() : root(nullptr) {}

    // funkce pro vlozeni nove polozky na sklad
    void vloz_na_sklad() {
        int id;
        string title, category;
        unsigned int ks;
        double price;

        // probehne kontrola vstupu od uzivatele
        check_numeric_input(id, "Zadejte ID výrobku: ");
        check_string_input(category, "Zadejte název výrobku: ");
        check_numeric_input(ks, "Zadejte počet na skladu: ");
        check_numeric_input(price, "Zadejte cenu výrobku: ");
        check_string_input(category, "Zadejte kategorii výrobku: ");

        root = vloz(root, id, title, ks, price, category);
        cout << "Položka byla přidána na sklad.\n";
    }

    // funkce pro vypis obsahu skladu
    void vypis() {
        if (root == nullptr) {
            cout << "Sklad je prázdný.\n";
        }
        else {
            vypis(root);
        }
    }

    // funkce pro vyhledani polozek podle kategorie
    void hledat_podle_kategorie() {
        string category;
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        check_string_input(category, "Zadejte kategorii: ");

        cout << "Výsledky vyhledávání:\n";
        vyhledat_kategorii(root, category);
    }

    // funkce pro vyhledani polozek podle ID
    void hledat_podle_id() {
        int id;
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        check_numeric_input(id, "Zadejte ID: ");

        Record* result = vyhledat_id(root, id);
        if (result) {
            char* buffer = ctime(&result->datum_posledniho_naskladneni);

            cout << "ID: " << result->id << ", Název: " << result->title << ", Kategorie: " << result->category
                << ", Počet kusů: " << result->ks << ", Cena: " << result->price
                << ", Datum naskladnění: " << buffer;
        }
        else {
            cout << "Položka nebyla nalezena.\n";
        }
    }

    void odstranit_polozku() {
        int id;
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        check_numeric_input(id, "Zadejte ID položky, kterou chcete odstranit: ");

        root = odstranit(root, id);
        cout << "Položka byla odstraněna (pokud existovala).\n";
    }
};

int main() {
    
    Sklad mujSklad;
    int choice;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. Přidat nové zboží\n";
        cout << "2. Výpis všech položek\n";
        cout << "3. Hledat podle kategorie\n";
        cout << "4. Hledat podle ID\n";
        cout << "5. Odstranit položku\n";
        cout << "6. Konec\n";
        cout << "=================\n";
        cout << "Vyberte možnost (1-6): ";
        cin >> choice;

        switch (choice) {
        case 1:
            mujSklad.vloz_na_sklad();
            break;
        case 2:
            mujSklad.vypis();
            break;
        case 3:
            mujSklad.hledat_podle_kategorie();
            break;
        case 4:
            mujSklad.hledat_podle_id();
            break;
        case 5:
            mujSklad.odstranit_polozku();
            break;
        case 6:
            cout << "Ukončuji program.\n";
            break;
        default:
            cout << "Neplatná volba. Zkuste to znovu.\n";
            break;
        }
    } while (choice != 6);

    return 0;
}
