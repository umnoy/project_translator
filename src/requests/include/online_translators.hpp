#ifndef ONLINE_TRANSLATORS_HPP
#define ONLINE_TRANSLATORS_HPP

#include "http_client.hpp"

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/json.hpp>
#include <boost/system/error_code.hpp>

namespace json = boost::json;

// Структура для хранения прочитанных API ключей и связанных параметров
struct ApiKeys {
    std::string yandex_key;
    std::string azure_key;
    std::string third_key;
    // Добавь другие ключи или параметры
};

struct TranslationResult {// Создаем коробку, которая будет хранить один результат.
    std::string translator_name; // translator_name - Имя переводчика (например, "Яндекс", "Azure"). Это текст.
    std::string translated_text; // translated_text - Сам переведенный текст. Это текст.
    // Можно добавить статус, например, успешно или ошибка.
    bool success; // success - Правда или ложь? Успешно ли получили перевод от этого источника.
    std::string error_message; // error_message - Если была ошибка, здесь текст ошибки.

    // Конструктор - инструкция, как собрать эту коробку.
    TranslationResult(const std::string& name, const std::string& text, bool s, const std::string& error = "") // Когда создаем коробку, даем ей имя, текст, статус и, возможно, ошибку.
        : translator_name(name), translated_text(text), success(s), error_message(error) {} // Записываем полученные данные внутрь коробки.
};

// класс для Управляющего Онлайн-Переводчиками.
class OnlineTranslatorsManager {  
private: 
    HttpClient http_client_; // http_client_ - Наш Телефон (объект класса HttpClient). Управляющий будет им пользоваться.
    ApiKeys api_keys_;
    // Тут можно хранить настройки для каждого переводчика (URL, ключи и т.д.).
    // Для простоты, пока не будем их хранить тут, но в реальном проекте это нужно.

public: 

    // Конструктор Управляющего. Как собрать Управляющего.
    // Объявляем конструктор. Он ничего не принимает, просто создает Управляющего.

    OnlineTranslatorsManager(const std::string &config_filepath);


    // Функция для получения переводов от всех онлайн-переводчиков.
    // Принимает текст, исходный и целевой языки.
    // Возвращает список результатов перевода (вектор TranslationResult).
    std::vector<TranslationResult> GetTranslations( // Объявляем функцию GetTranslations. Она вернет список коробок с переводами.
        const std::string& text,         // text - Текст, который нужно перевести.
        const std::string& source_lang,  // ource_lang - Язык, с которого переводим (например, "ru").
        const std::string& target_lang   // target_lang - Язык, на который переводим (например, "en").
    );

private: 

    // Функция для звонка конкретному онлайн-переводчику.
    // Это шаблон, который мы будем использовать для разных переводчиков.
    TranslationResult GetTranslationFromOne( // Объявляем функцию для звонка Одному переводчику. Вернет одну коробку с результатом.
        const std::string& translator_name, // Имя этого переводчика (для записи в результат).
        const std::string& url,             // Полный адрес API этого переводчика.
        const std::string& target, 
        http::verb method,
        const std::string& request_body,    // Тело запроса для этого переводчика (часто JSON).
        const std::vector<std::pair<std::string, std::string>>& headers // Заголовки, специфичные для этого переводчика (API ключи и т.д.).
    );

    // Функции для подготовки запроса и разбора ответа для каждого конкретного переводчика.
    // !!! Эти функции будут специфичны для каждого API и требуют изучения их документации !!!
    // Это просто ПРИМЕРЫ, как они МОГЛИ бы называться.
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>> // Функция, которая подготовит тело запроса и заголовки. Вернет пару: строка (тело) и список пар (заголовки).
        PrepareYandexRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang); // Для Яндекс.

    std::string ParseYandexResponse(const std::string& response_body); // Функция, которая найдет перевод в ответе от Яндекс.

    // Аналогично для Azure... (Пример)
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
        PrepareAzureRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang);

    std::string ParseAzureResponse(const std::string& response_body);

    // И для третьего переводчика... (Пример)
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
        PrepareThirdRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang);

    std::string ParseThirdResponse(const std::string& response_body);

}; 

#endif 






/*
Класс для работы с онлайн-переводчиками
class OnlineTranslators {
public:
    // Конструктор
    OnlineTranslators();
    
    // Получение перевода от Google Translate
    std::string translateGoogle(const std::string& text, 
                              const std::string& fromLang, 
                              const std::string& toLang);
    
    // Получение перевода от Yandex Translate
    std::string translateYandex(const std::string& text, 
                               const std::string& fromLang, 
                               const std::string& toLang);
    
    // Получение перевода от DeepL
    std::string translateDeepL(const std::string& text, 
                             const std::string& fromLang, 
                             const std::string& toLang);
    
    // Проверка доступности сети
    bool isNetworkAvailable() const;
    
private:
    std::unique_ptr<class HttpClient> httpClient_; // Указатель на HTTP клиент
    
    // API ключи (в реальном проекте должны храниться безопасно)
    const std::string googleApiKey_ = "your_google_api_key";
    const std::string yandexApiKey_ = "your_yandex_api_key";
    const std::string deeplApiKey_ = "your_deepl_api_key";
    
    // URL API переводчиков
    const std::string googleUrl_ = "translation.googleapis.com";
    const std::string yandexUrl_ = "translate.api.cloud.yandex.net";
    const std::string deeplUrl_ = "api-free.deepl.com";
};
*/
