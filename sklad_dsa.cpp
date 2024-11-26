#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Record {
public:
    int id;                        // Identifikační číslo zboží
    string title;                  // Název zboží
    unsigned int ks;               // Počet kusů na skladě
    double price;                  // Cena zboží
    string category;               // Kategorie zboží
    time_t datum_posledniho_naskladneni; // Datum posledního naskladnění
    Record* left;                  // Levý potomek
    Record* right;                 // Pravý potomek

    Record(int id, string title, unsigned int ks, double price, string category)
        : id(id), title(title), ks(ks), price(price), category(category), left(nullptr), right(nullptr) {
        time(&datum_posledniho_naskladneni); // Nastavení aktuálního data
    }
};

class Sklad {
private:
    Record* root;

    // Vložení nového záznamu do stromu
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
            // Pokud existuje položka se stejným ID, aktualizuje množství a datum
            node->ks += ks;
            time(&node->datum_posledniho_naskladneni);
        }
        return node;
    }

    // Vyhledání záznamu dle ID
    Record* najdi(Record* node, int id) {
        if (node == nullptr || node->id == id) {
            return node;
        }
        if (id < node->id) {
            return najdi(node->left, id);
        }
        return najdi(node->right, id);
    }

    // Výpis stromu (in-order traversal)
    void vypis(Record* node) {
        if (node != nullptr) {
            vypis(node->left);

            // Použití ctime_s pro bezpečný převod času na řetězec
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &node->datum_posledniho_naskladneni);

            cout << "ID: " << node->id << ", Název: " << node->title << ", Kategorie: " << node->category
                << ", Počet kusů: " << node->ks << ", Cena: " << node->price
                << ", Datum naskladnění: " << buffer;
            vypis(node->right);
        }
    }

    // Hledání dle kategorie (in-order traversal)
    void hledaniKategorie(Record* node, string category) {
        if (node != nullptr) {
            hledaniKategorie(node->left, category);
            if (node->category == category) {
                cout << "ID: " << node->id << ", Název: " << node->title
                    << ", Počet kusů: " << node->ks << ", Cena: " << node->price << endl;
            }
            hledaniKategorie(node->right, category);
        }
    }

    // Hledání nedostatkového zboží (in-order traversal)
    void hledaniNedostatkoveho(Record* node, unsigned int threshold) {
        if (node != nullptr) {
            hledaniNedostatkoveho(node->left, threshold);
            if (node->ks <= threshold) {
                cout << "ID: " << node->id << ", Název: " << node->title
                    << ", Počet kusů: " << node->ks << " (Nedostatek!)" << endl;
            }
            hledaniNedostatkoveho(node->right, threshold);
        }
    }

    // Najdi minimální prvek
    Record* najdiMin(Record* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    // Odstranění položky
    Record* smaz(Record* node, int id) {
        if (node == nullptr) {
            return nullptr;
        }
        if (id < node->id) {
            node->left = smaz(node->left, id);
        }
        else if (id > node->id) {
            node->right = smaz(node->right, id);
        }
        else {
            // Uzly s jedním nebo bez potomků
            if (node->left == nullptr) {
                Record* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr) {
                Record* temp = node->left;
                delete node;
                return temp;
            }
            // Uzly se dvěma potomky
            Record* temp = najdiMin(node->right);
            node->id = temp->id;
            node->title = temp->title;
            node->ks = temp->ks;
            node->price = temp->price;
            node->category = temp->category;
            node->datum_posledniho_naskladneni = temp->datum_posledniho_naskladneni;
            node->right = smaz(node->right, temp->id);
        }
        return node;
    }

public:
    Sklad() : root(nullptr) {}

    void vloz_na_sklad(int id, string title, unsigned int ks, double price, string category) {
        root = vloz(root, id, title, ks, price, category);
    }

    void najdi_polozku(int id) {
        Record* result = najdi(root, id);
        if (result != nullptr) {
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &result->datum_posledniho_naskladneni);

            cout << "Nalezeno: ID: " << result->id << ", Název: " << result->title
                << ", Počet kusů: " << result->ks << ", Cena: " << result->price
                << ", Kategorie: " << result->category
                << ", Datum: " << buffer << endl;
        }
        else {
            cout << "Položka s ID " << id << " nebyla nalezena." << endl;
        }
    }

    void smaz_polozku(int id) {
        root = smaz(root, id);
    }

    void vypis() {
        vypis(root);
    }

    void hledani_dle_kategorie(string category) {
        cout << "Položky v kategorii \"" << category << "\":" << endl;
        hledaniKategorie(root, category);
    }

    void hledani_nedostatkoveho(unsigned int threshold) {
        cout << "Nedostatkové zboží (<= " << threshold << " ks):" << endl;
        hledaniNedostatkoveho(root, threshold);
    }
};

int main() {
    

    Sklad mujSklad;

    // Vkládání položek
    mujSklad.vloz_na_sklad(1, "Kolo", 10, 1500.0, "Sport");
    mujSklad.vloz_na_sklad(2, "Šroub", 50, 5.5, "Nářadí");
    mujSklad.vloz_na_sklad(3, "Šroubovák", 5, 50.0, "Nářadí");
    mujSklad.vloz_na_sklad(4, "Chleba", 100, 25.0, "Potraviny");

    // Výpis skladu
    mujSklad.vypis();

    // Hledání dle kategorie
    mujSklad.hledani_dle_kategorie("Nářadí");

    // Hledání nedostatkového zboží
    mujSklad.hledani_nedostatkoveho(10);

    // Vyhledání a odstranění položky
    mujSklad.najdi_polozku(3);
    mujSklad.smaz_polozku(3);

    // Výpis skladu po odstranění
    mujSklad.vypis();

    return 0;
}
