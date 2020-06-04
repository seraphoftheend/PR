#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <regex>
#include <string>
#include <queue>

using namespace std;

queue<string> init_message(string to, string from, string subj, string message) {

    queue<string> msg;
    string s = "To: " + to + "\r\n";
    msg.push(s);
    s = "From: " + from + "\r\n";
    msg.push(s);
    s = "Subject: " + subj + "\r\n";
    msg.push(s);
    s = "\r\n";
    msg.push(s);
    msg.push(message);
    return msg;
}

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp) {

    queue<string>* msg = (queue<string>*)userp;
    string s;
    if (msg->empty()) {
        return 0;
    }
    else {
        s = msg->front();
        msg->pop();
    }

    memcpy(ptr, s.c_str(), s.size());
    return s.size();
}

size_t write_data(char* ptr, size_t size, size_t nmemb, FILE* data)
{
    return fwrite(ptr, size, nmemb, data);
}


int main() {

    CURL* curl;
    CURLcode res;
    struct curl_slist* recipients = NULL;

    curl = curl_easy_init();
    string to = "<csnow002@gmail.com>";
    string from = "<csnow002@gmail.com>";
    string subject = "TestTestTest";
    string message = "This is a test message!";

    queue<string> msg = init_message(to, from, subject, message);

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "smtp.gmail.com:587");              //URL и порт сервера
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);          //включить шифрование SSL/TLS
        curl_easy_setopt(curl, CURLOPT_USERNAME, "csnow002@gmail.com");         //email пользователя
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "****************");           //пароль пользователя
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());                //email отправителя
        recipients = curl_slist_append(recipients, to.c_str());                 //добавить получателя в список
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);                  //назначить список получателей
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);           //назначить функцию отправки данных
        curl_easy_setopt(curl, CURLOPT_READDATA, &msg);                         //указатель на сообщение, который будет передаваться в ф-цию отправки
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);                             //разрешаем отправку данных
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);                             //разрешаем вывод отладочной информации

        res = curl_easy_perform(curl);                                          //выполняем запрос

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        curl_slist_free_all(recipients);                                        //очищаем список получателей
    }

    curl_easy_reset(curl);                                                      //сбрасываем все параметры CURL handler 

    if (curl) {
        FILE* fp;
        if ((fopen_s(&fp,".\\Emails\\mail_info.txt", "wb")) != 0) {
            printf("Cannot open file.\n");
            return 1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "imaps:/imap.gmail.com:993/");
        curl_easy_setopt(curl, CURLOPT_USERNAME, "csnow002@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "***************");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE INBOX");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        fclose(fp);

        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        ifstream mailboxInfo(".\\Emails\\mail_info.txt");
        string s;
        smatch m;
        regex e("(\\d*)([ ]EXISTS)");   
        int mailindex = 0;

        if (mailboxInfo.good()) {
            while (getline(mailboxInfo, s)) {
                if (regex_search(s, m, e)) {
                    cout << m[0] << ' ' << m[1] << endl;
                    mailindex = stoi(m[1]);
                }
            }
        }

        curl_easy_reset(curl);

        curl_easy_setopt(curl, CURLOPT_USERNAME, "csnow002@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "**************");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        string basicURL = "imaps:/imap.gmail.com:993/INBOX/;MAILINDEX=";
        string concreteMailURL;
        string basicFileName=".\\Emails\\mail_";
        string concreteFileName;
        int mailcount = 10;

        for (int i = mailindex; i > mailindex - mailcount; i--) {
            concreteFileName = basicFileName + to_string(i) + ".txt";
            if ((fopen_s(&fp, concreteFileName.c_str() , "wb")) != 0) {
                printf("Cannot open file.\n");
                return 1;
            }

            concreteMailURL = basicURL + to_string(i);
            curl_easy_setopt(curl, CURLOPT_URL, concreteMailURL.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);
        }

        /* Always cleanup */
        curl_easy_cleanup(curl);
    }

    return (int)res;
}