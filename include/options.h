#define INFERNO_H
#ifdef INFERNO_H
#include <codecvt>
#include <locale>
#include <iostream>
#include <string>
#include <unistd.h>

class Node{
    public:
    char val;
    wchar_t wval;
    Node *next = NULL;

    Node(char val){
        this->val = val;
    }
    Node(wchar_t val){
        this->wval = val;
    }
};

class LinkedList{
private:
    int size = 0;
public:
    Node *head = NULL;
    
    int length(){
        return this->size;
    }

    void add(char value){
        Node *newVal = new Node(value);

        if(this->head == NULL){
            this->head = newVal;
            this->size++;
            return;
        }

        Node *cur = this->head;
        while(cur->next != NULL){
            cur = cur->next;
        }

        cur->next = newVal;
        this->size++;
    }

    void w_add(wchar_t value){
        Node *newVal = new Node(value);

        if(this->head == NULL){
            this->head = newVal;
            this->size++;
            return;
        }

        Node *cur = this->head;
        while(cur->next != NULL){
            cur = cur->next;
        }

        cur->next = newVal;
        this->size++;
    }

    std::string to_string(){
        std::string temp = "";
        Node *cur = this->head;
        while(cur != NULL){
            temp+=cur->val;
            cur = cur->next;
        }
        return temp;
    }

    std::wstring w_to_string(){
        std::wstring temp = L"";
        Node *cur = this->head;
        while(cur != NULL){
            temp+=cur->wval;
            cur = cur->next;
        }
        return temp;
    }

    void clear(){
        Node * cur;
        while(this->head != NULL){
            cur = this->head->next;
            delete [] this->head;
            this->head = cur;
            this->size--;
        }
    }
};

class StringMan{
public:
    //splits string at the first sighting of the delimiter into an array. Does not include the delimiter value.
//if no delimiter is found. the whole string is returned as an array.
    std::string* split(std::string str, char delimiter, const int length, const int limit=2){
        std::string *all = new std::string[limit+1];
        LinkedList *left = new LinkedList();
 
        int count = 0;

        for(int i = 0; i < length; i++){
            if((str)[i] == delimiter && count < limit){
                all[count] = left->to_string();
                left->clear();
                count++;
                continue;
            }

            left->add((str)[i]);
            
            if(i == length-1){
                all[count] = left->to_string();
                left->clear();
            }
        }
        left->clear();
        delete [] left;
        return all;
    }

    std::wstring* w_split(std::wstring str, wchar_t delimiter, const int length, const int limit=2){
        std::wstring *all = new std::wstring[limit+1];
        LinkedList *left = new LinkedList();
 
        int count = 0;

        for(int i = 0; i < length; i++){
            if((str)[i] == delimiter && count < limit){
                all[count] = left->w_to_string();
                left->clear();
                count++;
                continue;
            }

            left->w_add((str)[i]);
            
            if(i == length-1){
                all[count] = left->w_to_string();
                left->clear();
            }
        }
        left->clear();
        delete left;
        return all;
    }

    std::string deldup(std::string str, int length, bool specific = false, char ch = ' '){
        char prev;
        char cur;

        LinkedList *list = new LinkedList();
        bool flag = 0;

        if(!specific){
            if(length>1){
                for(int i = 1; i < length; i++){
                    prev = str[i-1];
                    cur = str[i];
                    flag = false;

                    if(prev == cur){
                        str[i-1] = ' ';
                        flag = true;
                    }
                    if(i == length-1){
                        list->w_add(str[i-1]);
                        list->add(str[i]);
                    }else{
                        if(flag){
                            continue;
                        }
                        list->add(str[i-1]);
                    }
                }
                return list->to_string();
            }else{
                return str;
            }
        }else{
            if(length>1){
                for(int i = 1; i < length; i++){
                    prev = str[i-1];
                    cur = str[i];
                    flag = false;
                    if(prev == cur && cur == ch){
                        str[i-1] = ' ';
                        flag = true;
                    }
                    if(i == length-1){
                        list->add(str[i]);
                    }else{
                        if(flag){
                            continue;
                        }
                        list->add(str[i-1]);
                    }
                    
                }
                return list->to_string();
            }else{
                return str;
            }
        }
    }

    std::wstring w_deldup(std::wstring str, int length, bool specific = false, std::wstring ch = L" "){
        wchar_t prev;
        wchar_t cur;

        LinkedList *list = new LinkedList();
        bool flag = 0;

        if(!specific){
            if(length>1){
                for(int i = 1; i < length; i++){
                    prev = str[i-1];
                    cur = str[i];
                    flag=false;

                    if(prev == cur){
                        str[i-1] = L' ';
                        flag=true;
                    }
                    if(i == length-1){
                        list->w_add(str[i-1]);
                        list->w_add(str[i]);
                    }else{
                        if(flag){
                            continue;
                        }
                        list->w_add(str[i-1]);
                    }
                }
                return list->w_to_string();
            }else{
                return str;
            }
        }else{
            if(length>1){
                for(int i = 1; i < length; i++){
                    prev = str[i-1];
                    cur = str[i];
                    flag = false;
                    if(prev == cur){
                        for(int k = 0; k < ch.size(); k++){
                            if(cur == ch[k]){
                                flag=true;
                                str[i-1] = L' ';
                                break;
                            }
                        }
                        //str[i-1] = L' ';
                        //flag=true;
                    }
                    if(i == length-1){
                        list->w_add(str[i-1]);
                        list->w_add(str[i]);
                    }else{
                        if(flag){
                            continue;
                        }
                        list->w_add(str[i-1]);
                    }
                    
                }
                return list->w_to_string();
            }else{
                return str;
            }
        }
    }

    std::string replace(std::string str, int length, char rep, char rep2){
        for(int i = 0; i < length; i++){
            if(str[i] == rep){
                if(rep2 == '\0'){
                    str[i] = ' ';
                }else{
                    str[i] = rep2;
                }
            }
        }
        return this->strip(str, length);
    }

    std::wstring w_replace(std::wstring str, int length, wchar_t rep, wchar_t rep2){
        for(int i = 0; i < length; i++){
            if(str[i] == rep){
                if(rep2 == '\0'){
                    str[i] = ' ';
                }else{
                    str[i] = rep2;
                }
            }
        }
        return this->w_strip(str, length);
    }

    std::wstring* w_split2(std::wstring str, const wchar_t *delimiters, const int length, const int limit=2){
        std::wstring *all = new std::wstring[limit+1];
        LinkedList *left = new LinkedList();
 
        int count = 0;

        bool skip = false;
        outter:for(int i = 0; i < length; i++){
            for(int k = 0; k < wcslen(delimiters); k++){
                if((str)[i] == delimiters[k] && count < limit){
                    if(left->w_to_string() == L""){
                        left->clear();
                        skip = true;
                        continue;
                    }
                    all[count] = left->w_to_string();
                    left->clear();
                    count++;
                    skip = true;
                    //continue;
                }
            }
            if(skip){
                skip = false;
                continue;
            } 
            /*if((str)[i] == delimiter && count < limit){
                all[count] = left->w_to_string();
                left->clear();
                count++;
                continue;
            }*/

            left->w_add((str)[i]);
            
            if(i == length-1){
                all[count] = left->w_to_string();
                left->clear();
            }
        }
        left->clear();
        delete left;
        return all;
    }

    bool wstrcmp(std::wstring in, std::wstring in2){
        std::wstring strip_in = this->w_strip(in, in.size());
        std::wstring strip_in2 = this->w_strip(in2, in2.size());

        if(strip_in.size() != strip_in2.size()){
            return false;
        }

        for(int i = 0; i < strip_in.size(); i++){
            if(strip_in[i] == strip_in2[i]){
                continue;
            }else{
                return false;
            }
        }
        return true;
    }

    bool strcmp(std::string in, std::string in2){
        if(in.size() != in2.size()){
            return false;
        }

        for(int i = 0; i < in.size(); i++){
            if(in[i] == in2[i]){
                continue;
            }else{
                return false;
            }
        }
        return true;
    }

    int length_wchar(wchar_t *val){
        int count = 0;
        while(true){
            if(val[count] == '\0'){
                break;
            }
            count+=1;
        }
        return count;
    }

    void println(std::string val){
        std::cout << val << "\n";
    }

    void wprintln(std::wstring val){
        std::wcout << val << L"\n";
    }

    std::string strip(std::string str, int length){
        char cur;
        char prev;
        bool char_found = false;
        bool is_space = false;
        short count = 0;

        LinkedList * list = new LinkedList();

        for(int i = 0; i < length; i++){
            if((str)[i] != ' '){
                char_found = true;
            }

            if(char_found){
                cur = (str)[i];

                if(cur == ' '){
                    is_space = true;
                    continue;
                }
                if(cur != ' '){
                    if(is_space){
                        list->add(' ');
                        count+=1;
                    }
                    is_space = false;
                    list->add((str)[i]);
                    (str)[count] = (str)[i];
                    (str)[i] = ' ';
                    count+=1;
                    continue;
                }
            }
        }
        //std::cout << list->length() << "\n";
        return list->to_string();
    }

    std::wstring w_strip(std::wstring str, int length){
        wchar_t cur;
        wchar_t prev;
        bool char_found = false;
        bool is_space = false;
        short count = 0;

        LinkedList * list = new LinkedList();

        for(int i = 0; i < length; i++){
            if((str)[i] != L' '){
                char_found = true;
            }

            if(char_found){
                cur = (str)[i];

                if(cur == L' '){
                    is_space = true;
                    continue;
                }
                if(cur != L' '){
                    if(is_space){
                        list->w_add(L' ');
                        count+=1;
                    }
                    is_space = false;
                    list->w_add((str)[i]);
                    (str)[count] = (str)[i];
                    (str)[i] = L' ';
                    count+=1;
                    continue;
                }
            }
        }
        //std::cout << list->length() << "\n";
        return list->w_to_string();
    }

    std::string remove_char(std::string *str, char val, int length){
        LinkedList *list = new LinkedList();
        for(int i = 0; i < length; i++){
            if((*str)[i] == val){
                continue;
            }
            list->add((*str)[i]);
        }
        return list->to_string();
    }

    std::string wstrtostr(const std::wstring &wstr)
    {
        // Convert a utf-16 to utf-8. all characters on windows are utf-16
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert; 
        std::string utf8 = convert.to_bytes(wstr);
        return utf8;
    }

    std::wstring strtowstr(const std::string str)
    {
        // Convert an ASCII string to a Unicode String
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert; 
        std::wstring utf8 = convert.from_bytes(str);
        //std::wstring wstr(utf16.begin(), utf16.end());
        return utf8;
    }
};

class Convertors{
public:
    std::string u32_to_str(const std::u32string str){
        try{
            std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> converter;
            return converter.to_bytes(str);
        }catch(std::range_error &e){
            return "";
        }
    }
    
    std::u32string str_to_u32(const std::string str){
        try{
            std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> converter;
            return converter.from_bytes(str);
        }catch(std::range_error &e){
            return NULL;
        }
    }

    std::string u16_to_str(const std::u16string str){
        try{
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            return converter.to_bytes(str);
        }catch(std::range_error &e){
            return "";
        }
    }
    
    std::u16string str_to_u16(const std::string str){
        try{
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            return converter.from_bytes(str);
        }catch(std::range_error &e){
            return NULL;
        }
    }

    std::string wstr_to_str(const std::wstring &wstr)
    {
        // Convert a utf-16 to utf-8. all characters on windows are utf-16
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert; 
        std::string utf8 = convert.to_bytes(wstr);
        return utf8;
    }

    std::wstring str_to_wstr(const std::string str)
    {
        // Convert an ASCII string to a Unicode String
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert; 
        std::wstring utf8 = convert.from_bytes(str);
        //std::wstring wstr(utf16.begin(), utf16.end());
        return utf8;
    }
};

#endif