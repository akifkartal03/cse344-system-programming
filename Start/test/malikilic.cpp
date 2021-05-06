#include<iostream>
#include<fstream>
#include<string>
using namespace std;
string pharmacy_list = "list_pharmacy.dat"; 
class pharmacy
{
 private:
    int pharmacy_id;
    string pharmacy_name;
    string pharmacy_adress;
 public:
    pharmacy();                                   // default constructor
    pharmacy(int id,string name, string adress);   // constructot with arg
    pharmacy(const pharmacy &other);                //copy constructor
    pharmacy &operator=(const pharmacy &other);     //assignment operator

    void create_pharmacy();
    void show_pharmacy();
    void pharmacy_drug_page();

    void set_id(int id_);
    void set_name(string name_);
    void set_adress(string adress_);

    int get_id();
    string get_name();
    string get_adress();
};

class drug
{
 private:
    int drug_id;
    int drug_number;
    double drug_price;
    string drug_name;
 public:
    drug();
    drug(int id , int number , double price , string name);
    drug(const drug &other);
    drug &operator=(const drug &other);

    void create_drug();
    void show_drug();
    void edit_drug();

    void set_id(int id);
    void set_number(int number);
    void set_price(double price);
    void set_name(string name);

    int get_id();
    int get_number();
    double get_price();
    string get_name();
};

//Function protatipe
void enter_pharmacy(int parola);
void create_pharmacy_list();
void add_pharmacy();
void add_drug();
void edit_drug_Dat();
void enter_customer();
void search_drug();
void list_pharmacy();
void show_any_pharmacy();
void drug_list_for_any_pharmacy();
void welcome();

int main()
{
    welcome();

    return 0;
}

pharmacy::pharmacy() 
{
    pharmacy_id = 0;
    pharmacy_name   = "Default";
    pharmacy_adress = "Default";
}
pharmacy::pharmacy(int id,string name, string adress)
{
    pharmacy_id = id;
    pharmacy_name = name;
    pharmacy_adress = adress;
}
pharmacy::pharmacy(const pharmacy &other)                 
{
    pharmacy_id = other.pharmacy_id;
    pharmacy_name = other.pharmacy_name;
    pharmacy_adress = other.pharmacy_adress;
    
}
pharmacy & pharmacy::operator=(const pharmacy &other)
{
    pharmacy_id = other.pharmacy_id;
    pharmacy_name = other.pharmacy_name;
    pharmacy_adress = other.pharmacy_adress;
    return (*this);
}

void pharmacy::create_pharmacy()
{  
    cout<<"Girilen Eczanenin ID Numarasi: ";
    cin>>pharmacy_id;
    cout<<"Girilen Eczanenin Ismi: ";
    cin>>pharmacy_name;
    cout<<"Girilen Eczanenin Adresi: ";
    cin>>pharmacy_adress;  
}
void pharmacy::show_pharmacy()
{
    cout
    <<"Girilen Eczanenin Ismi: "<<pharmacy_name<<endl
    <<"Girilen Eczanenin ID Numarasi: "<<pharmacy_id<<endl
    <<"Girilen Eczanenin Adresi: "<<pharmacy_adress;
}
void pharmacy::pharmacy_drug_page()
{
    string text;
    string file_name;
    file_name = pharmacy_name+".dat";
    ifstream file2(file_name); //create or open eczane file
    while (getline(file2,text)){cout<<text;}
    file2.close();
}

void pharmacy::set_id(int id_){pharmacy_id = id_;}
void pharmacy::set_name(string name_){pharmacy_name=name_;}
void pharmacy::set_adress(string adress_){pharmacy_adress = adress_;}

int pharmacy::get_id(){return pharmacy_id;}
string pharmacy::get_name(){return pharmacy_name;}
string pharmacy::get_adress(){ return pharmacy_adress;}

//ilac functions
drug::drug()
{
    drug_id = 0;
    drug_number = 0;
    drug_price = 0;
    drug_name = "";
    
}
drug::drug(int id, int number,double price,string name )
{
    drug_id = id;
    drug_number = number;
    drug_price = price;
    drug_name = name;
}
drug::drug(const drug &other)
{
    drug_id = other.drug_id;
    drug_number = other.drug_number;
    drug_price = other.drug_price;
    drug_name = other.drug_name;
}
drug &drug::operator=(const drug &other)
{
    drug_id = other.drug_id;
    drug_number = other.drug_number;
    drug_price = other.drug_price;
    drug_name = other.drug_name;
    return (*this);
}

void drug::create_drug()
{
    cout<<"Girilen Ilacin ID Numarasi: ";  cin>>drug_id;
    cout<<"Girilen Ilacin Sayisi: ";       cin>>drug_number;
    cout<<"Girilen Ilacin Fiyati: ";       cin>>drug_price;
    cout<<"Girilen Ilacin Ismi: ";         cin>>drug_name;
}
void drug::show_drug()
{
    cout<<"Girilen Ilacin Ismi: "<<drug_name<<endl<< "Id: "<<drug_id<<endl<< "Sayi: "<<drug_number<<endl<< "Fiyat: "<<drug_price;
}
void drug::edit_drug()
{
    cout<<"Yeni ilac id: ";       cin>>drug_id;
    cout<<"Yeni ilac sayisi: ";   cin>>drug_number;
    cout<<"Yeni ilac fiyati: ";   cin>>drug_price;
    cout<<"Yeni ilac ismi: ";     cin>>drug_name;
}

void drug::set_id(int id){drug_id = id;}
void drug::set_number(int number){drug_number=number;}
void drug::set_price(double price){drug_price=price;}
void drug::set_name(string name){drug_name=name;}

int drug::get_id(){return drug_id;}
int drug::get_number(){return drug_number;}
double drug::get_price(){return drug_price;}
string drug::get_name(){return drug_name;}

//other function
void enter_pharmacy(int pasword)
{
    if(pasword==171024095)
        while (1)
        {
            int choice;
            cout<<"\nEczane Girisi\n"<<"1. Eczane Listesi Olustur\n"<<"2. Eczane Ekle\n"<<"3. Ilac Ekle\n"<<"4. Ilac Duzenle\n"<< "5. Ust menu\n"<<"secim: ";
            cin>>choice;
            
            if (choice==1)
                create_pharmacy_list();
            else if(choice==2)
                add_pharmacy();
            else if(choice==3)
                add_drug();
            else if(choice==4)
                edit_drug_Dat();
            else if(choice==5)
            {
                cout<<"Bir onceki menuye geldin.\n";
                break;
            }
            else
                cout<<"Hata!";
        }
    else
        cout<<"Hatali �ifre girilmi�tir!: ";    
    
}
void enter_customer()
{
    while (1)
        {
            int choice;
            cout<<"\nMusteri Girisi\n"<<"1. Ilac Ara\n"<<"2. Eczane Listesi\n"<< "3. Herhangi bir Eczaneyi Goster\n"<<"4. Herhangi bir Eczaneye ait Ilac Listesi\n"<<"5. Ust menu\n"<<"secim: ";
            cin>>choice;
            
            if (choice==1)
                search_drug();
            else if(choice==2)
                list_pharmacy();
            else if(choice==3)
                show_any_pharmacy();
            else if(choice==4)
                drug_list_for_any_pharmacy();
            else if(choice==5)
            {
                cout<<"Bir onceki menuye geldin.\n";
                break;
            }
            else
                cout<<"Hata!"; 
        } 
}
void welcome()
{
    int choice;
    int pasword;
    while (1)
    {
        cout<<"1. Eczane Girisi\n"
            <<"2. Musteri Girisi\n"
            <<"3. Cikis\n"
            <<"\nSecim: ";
        cin>>choice;

        if(choice==1)
        {
            cout<<"L�tfen Parola giriniz!: ";
            cin>>pasword;
            enter_pharmacy(pasword);
        }
        else if(choice==2)
            enter_customer();
        else if(choice==3)
            break;
        else
            cout<<"Hatal� bir i�lem yap�ld�.\n";    
    }
    
}
void create_pharmacy_list()
{
    pharmacy p;
    p.create_pharmacy();
    fstream file;
    file.open("list_pharmacy.dat",std::ios_base::app);
    file<<p.get_id()<<" "<<p.get_name()<<" "<<p.get_adress()<<" ";
    file.close();

    string filename;
    filename = p.get_name() + "drug.dat";
    ofstream file2 (filename);
    file2<<"";
    file2.close();
}
void add_pharmacy()
{
    pharmacy p;
    p.create_pharmacy();
    fstream file;
    file.open("list_pharmacy.dat",std::ios_base::app);
    file<<p.get_id()<<" "<<p.get_name()<<" "<<p.get_adress()<<" ";
    file.close();

    string filename;
    filename = p.get_name() + "drug.dat";
    ofstream file2 (filename);
    file2<<"";
    file2.close();
}
void add_drug()
{
    drug product;
    product.create_drug();
    string pharmacy_name;
    cout<<"Bir eczane giriniz: ";
    cin>>pharmacy_name;
    fstream file;
    file.open(pharmacy_name+"drug.dat",std::ios_base::app);
    file<<product.get_id()<<" "<<product.get_name()<<" "<<product.get_price()<<" "<<product.get_number()<<" ";
    file.close();

}
void edit_drug_Dat()
{
    string pharmacy_name,store_adress,repeat,temporary,search;
    int i=0;
    bool a,b,c,d = false;
    cout<<"ilac stoklar�:\n ";
    list_pharmacy();
    cin>>pharmacy_name;
    cout<<"ilac id: ";
    cin>>search;
    store_adress=pharmacy_name+"drug.dat";
    fstream file(store_adress);
    while (getline(file,repeat,' '))
    {
        i++;
        if(i==1 && repeat==search)
        {
            cout<<"Gecmis id numarasi: "<<repeat<<endl<<"Yeni id: ";
            cin>>repeat;
            a = true;
        }
        if(i==2 && a)
        {
            cout<<"Gecmis isim: "<<repeat<<endl<<"Yeni isim: ";
            cin>>repeat;
            b = true;
            a = false;
        }
        if(i==3 && b)
        {
            cout<<"Gecmis fiyat: "<<repeat<<endl<<"Yeni fiyat: ";
            cin>>repeat;
            c = true;
            b = false;
        }
        if(i==4 && c)
        {
            cout<<"Gecmis adet: "<<repeat<<endl<<"Yeni adet: ";
            cin>>repeat;
            i=0;
            c = false;
        }
        
        temporary = temporary + " " + repeat;
    }
    file.close();
    file.open(store_adress);
    file<<temporary;
    file.close();
}

//customer funct
void list_pharmacy()
{
    string output;
    ifstream file(pharmacy_list);

    while (getline(file,output,' '))
    {
        cout<<output<<endl;
    }
    file.close();
}
void search_drug()
{
    string search;
    cout<<"Hangi ilaca ulasmak istersiniz? ";
    cin>>search;
    string pharmacy_name;
    ifstream file(pharmacy_list);
    string drug_file,drug_name;
    int i = 0;
    cout<<search<<" Aramakta oldugunuz ilacin bulundugu eczanelerin listesi:\n";
    while (getline(file,pharmacy_name,' '))
    {
        i++;
        if(i==2)
        {
            drug_file = pharmacy_name + "drug.dat";
            ifstream file2(drug_file);
            while (getline(file2,drug_name,' '))
            {
                if(search == drug_name)
                    cout<<pharmacy_name<<endl;
            }
            file2.close();
        }
        if(i==3)
            i=0;   
    }
    file.close();
}
void show_any_pharmacy()
{
    string finder,search;
    cout<<"Bulmak istedigin eczaneyi gir: ";
    cin>>search;
    pharmacy repeat;
    ifstream file(pharmacy_list);
    int i = 0;
    while (getline(file,finder,' '))
    {
        if(i==0)
            repeat.set_id(atoi(finder.c_str()));
        if(i==1)
            repeat.set_name(finder);
        if(i==2)
        {
            i=-1;
            repeat.set_adress(finder);
            if(repeat.get_name()==search)
            {
                repeat.show_pharmacy();
                break;
            } 
        }
        i++;
    }
    
    file.close();

}
void drug_list_for_any_pharmacy()
{
    string finder,search;
    string newpharmacy;
    cout<<"Depo gormek icin eczane ismi giriniz: ";
    cin>>newpharmacy;
    string pharmacy_store = newpharmacy+"drug.dat";
    ifstream file(pharmacy_store);
    int i = 0;

    while (getline(file,finder,' '))
    {
        cout<<finder<<endl;
        if(i==3)
        {
            cout<<endl;
            i=-1;
        }
        i++;
    }
    file.close();
}
