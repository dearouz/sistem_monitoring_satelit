#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <vector>
#include <sstream>

using namespace std;

struct Satelit {
    int id;
    string nama;
    string orbit;
    string masalah;
    string status;
};

struct Node {
    Satelit data;
    Node* left;
    Node* right;
};

struct Action {
    string type; // PROCESS / FINISH
    int id;
};

Node* root = NULL;
queue<int> antrian;
stack<Action> riwayat;
vector<int> idDalamQueue;

// ================= BST =================

Node* createNode(int id, string nama, string orbit, string masalah, string status = "Menunggu Monitoring") {
    Node* newNode = new Node;
    newNode->data.id = id;
    newNode->data.nama = nama;
    newNode->data.orbit = orbit;
    newNode->data.masalah = masalah;
    newNode->data.status = status;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* search(Node* root, int id) {
    if (root == NULL || root->data.id == id)
        return root;

    if (id < root->data.id)
        return search(root->left, id);

    return search(root->right, id);
}

Node* insertNode(Node* root,
                 int id,
                 string nama,
                 string orbit,
                 string masalah,
                 string status = "Menunggu Monitoring",
                 bool tampilPesan = true) {

    if (root == NULL) {

        if (tampilPesan)
            cout << "\nSatelit berhasil didaftarkan\n";

        return createNode(id, nama, orbit, masalah, status);
    }

    if (id < root->data.id)
        root->left = insertNode(root->left,
                                id,
                                nama,
                                orbit,
                                masalah,
                                status,
                                tampilPesan);

    else if (id > root->data.id)
        root->right = insertNode(root->right,
                                 id,
                                 nama,
                                 orbit,
                                 masalah,
                                 status,
                                 tampilPesan);

    else
        cout << "\nID sudah digunakan\n";

    return root;
}

void inorder(Node* root) {
    if (root != NULL) {
        inorder(root->left);

        cout << root->data.id << "  | "
             << root->data.nama << " | "
             << root->data.orbit << " | "
             << root->data.masalah << " | "
             << root->data.status << endl;

        inorder(root->right);
    }
}

Node* minValueNode(Node* node) {
    Node* current = node;

    while (current && current->left != NULL)
        current = current->left;

    return current;
}

Node* deleteNode(Node* root, int id) {
    if (root == NULL)
        return root;

    if (id < root->data.id)
        root->left = deleteNode(root->left, id);

    else if (id > root->data.id)
        root->right = deleteNode(root->right, id);

    else {
        if (root->left == NULL) {
            Node* temp = root->right;
            delete root;
            return temp;
        }

        else if (root->right == NULL) {
            Node* temp = root->left;
            delete root;
            return temp;
        }

        Node* temp = minValueNode(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data.id);
    }

    return root;
}

// ================= TUNGGU =================

void tunggu() {
    cout << "\nTekan ENTER untuk kembali ke menu...";
    cin.ignore();
    cin.get();
}

void tungguTanpaIgnore() {
    cout << "\nTekan ENTER untuk kembali ke menu...";
    cin.get();
}

// ================= QUEUE =================

bool sudahDalamQueue(int id) {
    for (int x : idDalamQueue) {
        if (x == id)
            return true;
    }
    return false;
}

void tambahAntrian() {
    int id;
    cout << "Masukkan ID satelit: ";
    cin >> id;

    Node* satelit = search(root, id);

    if (satelit == NULL) {
        cout << "\nSatelit tidak ditemukan\n";
        return;
    }

    if (sudahDalamQueue(id)) {
        cout << "\nSatelit sudah ada dalam antrian monitoring\n";
        return;
    }

    if (satelit->data.status != "Menunggu Monitoring") {
        cout << "\nSatelit tidak dapat masuk antrian\n";
        return;
    }

    antrian.push(id);
    idDalamQueue.push_back(id);

    cout << "\nSatelit " << satelit->data.nama
         << " ditambahkan ke antrian monitoring\n";
}

void prosesMonitoring() {
    if (antrian.empty()) {
        cout << "\nTidak ada satelit dalam antrian monitoring\n";
        return;
    }

    int id = antrian.front();
    antrian.pop();

    idDalamQueue.erase(idDalamQueue.begin());

    Node* satelit = search(root, id);

    satelit->data.status = "Sedang Dianalisis";

    Action act;
    act.type = "PROCESS";
    act.id = id;

    riwayat.push(act);

    cout << "\nSatelit "
         << satelit->data.nama
         << " sedang dianalisis\n";
}

void lihatAntrian() {
    if (antrian.empty()) {
        cout << "Antrian monitoring kosong\n";
        return;
    }

    queue<int> temp = antrian;
    int posisi = 1;

    while (!temp.empty()) {
        int id = temp.front();
        temp.pop();

        Node* satelit = search(root, id);

        cout << posisi << ". "
             << satelit->data.nama
             << " (" << satelit->data.id << ")\n";

        posisi++;
    }
}

// ================= STACK UNDO =================

void selesaiMonitoring() {
    int id;

    cout << "Masukkan ID satelit: ";
    cin >> id;

    Node* satelit = search(root, id);

    if (satelit == NULL) {
        cout << "\nSatelit tidak ditemukan\n";
        return;
    }

    if (satelit->data.status != "Sedang Dianalisis") {
        cout << "\nSatelit belum dalam proses analisis\n";
        return;
    }

    satelit->data.status = "Selesai Monitoring";

    Action act;
    act.type = "FINISH";
    act.id = id;

    riwayat.push(act);

    cout << "\nMonitoring satelit "
         << satelit->data.nama
         << " selesai\n";
}

void undoAksi() {
    if (riwayat.empty()) {
        cout << "\nTidak ada aksi untuk di-undo\n";
        return;
    }

    Action last = riwayat.top();
    riwayat.pop();

    Node* satelit = search(root, last.id);

    if (satelit == NULL) {
        cout << "\nData satelit tidak ditemukan\n";
        return;
    }

    if (last.type == "PROCESS") {
        satelit->data.status = "Menunggu Monitoring";

        antrian.push(last.id);
        idDalamQueue.push_back(last.id);

        cout << "\nUndo: Satelit "
             << satelit->data.nama
             << " dikembalikan ke antrian monitoring\n";
    }

    else if (last.type == "FINISH") {
        satelit->data.status = "Sedang Dianalisis";

        cout << "\nUndo: Status satelit "
             << satelit->data.nama
             << " dikembalikan ke Sedang Dianalisis\n";
    }
}

// ================= SORTING =================

void simpanData(Node* root, vector<Satelit>& data) {
    if (root != NULL) {
        simpanData(root->left, data);
        data.push_back(root->data);
        simpanData(root->right, data);
    }
}

void bubbleSortNama(vector<Satelit>& data) {
    int n = data.size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (data[j].nama > data[j + 1].nama) {
                swap(data[j], data[j + 1]);
            }
        }
    }

    cout << "\nData diurutkan berdasarkan nama satelit\n";

    for (Satelit s : data) {
        cout << s.id << " | "
             << s.nama << " | "
             << s.orbit << " | "
             << s.masalah << " | "
             << s.status << endl;
    }
}

void selectionSortID(vector<Satelit>& data) {
    int n = data.size();

    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;

        for (int j = i + 1; j < n; j++) {
            if (data[j].id < data[minIndex].id) {
                minIndex = j;
            }
        }

        swap(data[i], data[minIndex]);
    }

    cout << "\nData diurutkan berdasarkan ID satelit\n";

    for (Satelit s : data) {
        cout << s.id << " | "
             << s.nama << " | "
             << s.orbit << " | "
             << s.masalah << " | "
             << s.status << endl;
    }
}

// ================= FILE HANDLING =================

void tulisFile(Node* root, ofstream& file) {
    if (root != NULL) {
        tulisFile(root->left, file);

        file << root->data.id << "|"
             << root->data.nama << "|"
             << root->data.orbit << "|"
             << root->data.masalah << "|"
             << root->data.status << endl;

        tulisFile(root->right, file);
    }
}

void simpanKeFile() {
    ofstream file("data_satelit.txt");

    if (!file) {
        cout << "File gagal dibuka\n";
        return;
    }

    tulisFile(root, file);
    file.close();

    cout << "Data satelit berhasil disimpan ke file\n";
}

void bacaDariFile() {
    ifstream file("data_satelit.txt");

    if (!file) {
        cout << "File data_satelit.txt tidak ditemukan\n";
        return;
    }

    root = NULL;

    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, nama, orbit, masalah, status;

        getline(ss, idStr, '|');
        getline(ss, nama, '|');
        getline(ss, orbit, '|');
        getline(ss, masalah, '|');
        getline(ss, status, '|');

        int id = stoi(idStr);

        root = insertNode(root, id, nama, orbit, masalah, status, false);
    }

    file.close();

    cout << "\nData satelit berhasil dibaca dari file\n";
}

// ================= MAIN =================

int main() {
    system("cls");

    int pilihan;

    do {
        system("cls");

        cout << "\n===== SISTEM MONITORING SATELIT =====\n";
        cout << "1. Daftarkan Satelit\n";
        cout << "2. Tampilkan Data Satelit\n";
        cout << "3. Tambah ke Antrian Monitoring\n";
        cout << "4. Proses Monitoring Satelit\n";
        cout << "5. Selesaikan Monitoring\n";
        cout << "6. Sorting Data Satelit\n";
        cout << "7. Undo Aksi\n";
        cout << "8. Lihat Antrian Monitoring\n";
        cout << "9. Hapus Data Satelit\n";
        cout << "10. Simpan Data ke File\n";
        cout << "11. Baca Data dari File\n";
        cout << "0. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan;

        switch (pilihan) {

        case 1: {
            system("cls");

            cout << "------------------------\n";
            cout << "1. Daftarkan Satelit\n";
            cout << "------------------------\n";

            int id;
            string nama, orbit, masalah;

            cout << "ID Satelit: ";
            cin >> id;
            cin.ignore();

            cout << "Nama Satelit: ";
            getline(cin, nama);

            cout << "Jenis Orbit: ";
            getline(cin, orbit);

            cout << "Masalah Satelit: ";
            getline(cin, masalah);

            root = insertNode(root, id, nama, orbit, masalah);
            tungguTanpaIgnore();
            break;
        }

        case 2:
            system("cls");

            cout << "--------------------------\n";
            cout << "2. Tampilkan Data Satelit\n";
            cout << "--------------------------\n";

            if (root == NULL) {
                cout << "Belum ada data satelit\n";
            } else {
                cout << "ID | Nama | Orbit | Masalah | Status\n";
                cout << "--------------------------------------\n";
                inorder(root);
            }

            tunggu();
            break;

        case 3:
            system("cls");

            cout << "--------------------------------\n";
            cout << "3. Tambah ke Antrian Monitoring\n";
            cout << "--------------------------------\n";

            tambahAntrian();
            tunggu();
            break;

        case 4:
            system("cls");

            cout << "-----------------------------\n";
            cout << "4. Proses Monitoring Satelit\n";
            cout << "-----------------------------\n";

            prosesMonitoring();
            tunggu();
            break;

        case 5:
            system("cls");

            cout << "-------------------------\n";
            cout << "5. Selesaikan Monitoring\n";
            cout << "-------------------------\n";

            selesaiMonitoring();
            tunggu();
            break;

        case 6: {
            system("cls");

            cout << "------------------------\n";
            cout << "6. Sorting Data Satelit\n";
            cout << "------------------------\n";

            vector<Satelit> data;
            simpanData(root, data);

            if (data.empty()) {
                cout << "Belum ada data satelit\n";
                tunggu();
                break;
            }

            int pilihSort;

            cout << "1. Bubble Sort berdasarkan Nama\n";
            cout << "2. Selection Sort berdasarkan ID\n";
            cout << "Pilihan: ";
            cin >> pilihSort;

            if (pilihSort == 1)
                bubbleSortNama(data);
            else if (pilihSort == 2)
                selectionSortID(data);
            else
                cout << "Pilihan sorting tidak valid\n";

            tunggu();
            break;
        }

        case 7:
            system("cls");

            cout << "--------------\n";
            cout << "7. Undo Aksi\n";
            cout << "--------------\n";

            undoAksi();
            tunggu();
            break;

        case 8:
            system("cls");

            cout << "----------------------------\n";
            cout << "8. Lihat Antrian Monitoring\n";
            cout << "----------------------------\n";

            lihatAntrian();
            tunggu();
            break;

        case 9: {
            system("cls");

            cout << "----------------------\n";
            cout << "9. Hapus Data Satelit\n";
            cout << "----------------------\n";

            int id;

            cout << "Masukkan ID satelit: ";
            cin >> id;

            Node* cek = search(root, id);

            if (cek == NULL) {
                cout << "Satelit tidak ditemukan\n";
            } else {
                root = deleteNode(root, id);
                cout << "Data satelit berhasil dihapus\n";
            }

            tunggu();
            break;
        }

        case 10:
            system("cls");

            cout << "------------------------\n";
            cout << "10. Simpan Data ke File\n";
            cout << "------------------------\n";

            simpanKeFile();
            tunggu();
            break;

        case 11:
            system("cls");

            cout << "------------------------\n";
            cout << "11. Baca Data dari File\n";
            cout << "------------------------\n";

            bacaDariFile();
            tunggu();
            break;

        case 0:
            cout << "Program selesai\n";
            break;

        default:
            system("cls");

            cout << "Pilihan tidak valid\n";
            tunggu();
        }

    } while (pilihan != 0);

}
