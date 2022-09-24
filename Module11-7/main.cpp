/*
#include <fstream>
#include "iostream"

using namespace std;

void write_arr(const string& filename, const int* arr, const int n)
{
    fstream fs;
    
    fs.open(filename, fstream::out);
    if(fs.is_open())
    {
        fs << n << '\n';
        for (int i = 0; i < n; i++)
        {
            fs << arr[i] << ' ';
        }
        fs << 'n';
        fs.close();
    }
}

void read_arr(const string& filename, int*&arr, int& n)
{
    fstream fs;
    
    fs.open(filename, fstream::in);
    if (fs.is_open())
    {
        fs >> n;
        arr = new int[n];
        for(int i = 0; i < n; i++)
        {
            fs >> arr[i];
        }
        fs.close();
    }
}

int main ()
{
    string filename = "filaname1.txt";
    const size_t gigabyte = 1073741824; // Bytes per gigabyte
    const size_t size = 2 * gigabyte/sizeof(int); // Want a 2 GB file
    int* rand_arr = new int[size];  
    
    srand(time(nullptr));
    int lef_border = 0;
    int range_len = 10000;
    for (int i = 0; i < size; i++)
    {
        rand_arr[i] = lef_border + rand()%range_len;
    }
    
    write_arr(filename, rand_arr, size);

    int* array_from_file = nullptr;
   
    int n = 0;
   
    read_arr(filename, array_from_file, n);
    
    delete[] rand_arr;
    delete[] array_from_file;
   
    return 0;
}
*/
/*
 В качестве итогового задания реализуйте следующий проект: сгенерируйте большой файл (например, 2 Гб) и отсортируйте все числа, записанные в этом файле.
 Ограничение: вы не можете прочитать весь файл за раз!
 
 Вы можете пойти двумя путями:

 Создать много файлов, затем отсортировать их и соединить в один.
 Создать только два файла соединить их в один, затем еще два и соединить их опять в один, затем соединить оба полученных файла в один.
 
 Обратите внимание на флаг ofstream::trunc, он описан в документации, ссылка на которую была приведена.

 Используйте функцию для удаления файла и\или копирования файла.

 Чтобы прочитать, файл не зная количество элементов, используйте конструкцию while (!fs.eof()){}.
 */

//вариант предложенный на курсе

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <cstdlib>
#include <chrono>

using namespace std;

#define PART_SIZE 500000
#define RAND_ARR_SIZE 20000000 // ~100 mb
// #define RAND_ARR_SIZE 100000000 // ~500 mb
// #define RAND_ARR_SIZE 200000000 // ~1 gb
// #define RAND_ARR_SIZE 400000000 // ~2 gb


void print(int* arr, int n)
{
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int*& arr, int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);
  
    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int*& arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void merge_to_file(const int* arr1, const int* arr2, int sz1, int sz2)
{

    fstream temp;
    const int* first;
    const int* second;

    temp.open("temp_1_file.txt", fstream::out | std::ofstream::trunc);

    if (arr1[0] < arr2[0]) {
        first = arr1;
        second = arr2;
    }else {
        first = arr2;
        second = arr1;
        swap(&sz1, &sz2); // becouse swap arrays;
    }
    
    if (temp.is_open())
    {
        int i = 0;
        int j = 0;
        
        while (i < sz1 && j < sz2){
            if (first[i] < second[j])
                temp << first[i++] << ' ';
            else if (first[i] == second[j])
            {
                temp << first[i++] << ' ';
                temp << second[j++] << ' ';
            }
            else
                temp << second[j++] << ' ';
        }

        while (i < sz1)
            temp << first[i++] << ' ';

        while (j < sz2)
            temp << second[j++] << ' ';

       temp.close();
    }
}

void merge_files()
{
    fstream res;
    fstream temp1;
    fstream temp2;

    temp1.open("temp_1_file.txt", fstream::in);
    res.open("res_file.txt", fstream::in);
    temp2.open("temp_2_file.txt", fstream::out | ofstream::trunc); // open and clear

    if (!temp1.is_open() || !temp2.is_open() || !res.is_open())
        return;

    int temp1_value;
    int res_value;
        
    temp1 >> temp1_value;
    res >> res_value;
    while (!temp1.eof() && !res.eof()) {
        if (temp1_value <= res_value) {
            temp2 << temp1_value << ' ';
            temp1 >> temp1_value;
        }
        else {
            temp2 << res_value << ' ';
            res >> res_value;
        }
    }

    while (!res.eof()) {
        temp2 << res_value << ' ';
        res >> res_value;
    }

    while (!temp1.eof()) {
        temp2 << temp1_value << ' ';
        temp1 >> temp1_value;
    }

    temp1.close();
    temp2.close();
    res.close();
    
    // delete content of file
    res.open("res_file.txt", std::ofstream::out | std::ofstream::trunc);
    if (res.is_open())
        res.close();

    // copy result to result file
    if (!std::__fs::filesystem::copy_file("temp_2_file.txt", "res_file.txt",
        std::__fs::filesystem::copy_options::overwrite_existing))
        return;
}

// return the size of readed part
int read_part_arr(fstream& fs, int*& arr)
{
    arr = new int[PART_SIZE];
    int* tmp_arr;
    int i;
    for (i = 0; i < PART_SIZE && !fs.eof(); i++)
        fs >> arr[i];

    if (i == 1){
        delete[] arr;
        return 0;
    }

    if (i != PART_SIZE){
        tmp_arr = new int[i];
        for (size_t j = 0; j < i; j++)
            tmp_arr[j] = arr[j];

        delete[] arr;
        arr = tmp_arr;
        return i - 1;
    }

    return PART_SIZE;
}

void sort_func(const string& filename){
    
    fstream fs;
    fs.open(filename, fstream::in);

    if(fs.is_open())
    {
        while (!fs.eof())
        {
            int* part_1;
            int* part_2;

            int size_1 = read_part_arr(fs, part_1);
            int size_2 = read_part_arr(fs, part_2);
            if (size_1 == 0 || size_2 == 0)
                return;
            cout << " size_1 = " << size_1 << " size_2 = " << size_2 << endl;
            quickSort(part_1, 0, size_1 - 1);
            quickSort(part_2, 0, size_2 - 1);
            merge_to_file(part_1, part_2, size_1, size_2);
            merge_files();
        }
        fs.close();
    }
}

void write_rand_arr(const string& filename)
{
    fstream fs;

    srand(time(nullptr));
    int lef_border = -100;
    int range_len = 50000; // правая граница = range_len + left_border

    fs.open(filename, fstream::out | ofstream::trunc);
    if(fs.is_open())
    {
       for (int i = 0; i < RAND_ARR_SIZE; i++)
           fs << (lef_border + rand() % range_len) << ' ';

       fs.close();
    }
}

void read_arr(const string& filename, int*& arr, const int& n)
{
    fstream fs;
 
    fs.open(filename, fstream::in);
    if(fs.is_open()) // проверяем что файл успешно открыт
    {
        arr = new int[n];
        for (int i = 0; i < n; i++)
            fs >> arr[i];

        fs.close();
    }
}

int main(int argc, char const *argv[])
{

    string filename = "array_data.txt";

    write_rand_arr(filename);
    cout << "Genetrating data is done!" << endl;

    fstream res;
    res.open("res_file.txt", fstream::out | ofstream::trunc);
    res.close();

    auto start = chrono::high_resolution_clock::now();
    sort_func(filename);
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time: " << elapsed.count() << " sec" << endl;

    return 0;
}
