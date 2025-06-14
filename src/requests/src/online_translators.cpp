#include "online_translators.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept> 
#include <utility> 

#include <boost/json.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/system/system_error.hpp>
#include <boost/beast/http/verb.hpp> 

namespace json = boost::json;
namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ip = net::ip;

OnlineTranslatorsManager::OnlineTranslatorsManager(const std::string& config_filepath) {     
    api_keys_ = load_api_keys_from_file(config_filepath); 
    std::cout << "The API keys have been successfully loaded from the file: " << config_filepath << std::endl;
}


// Реализация функции Получить Переводы.
std::vector<TranslationResult> OnlineTranslatorsManager::GetTranslations(
    const std::string& text,
    const std::string& source_lang,
    const std::string& target_lang)
{
    std::vector<TranslationResult> results; 
// ------ Яндекс ------
    try { 
        std::pair<std::string, std::vector<std::pair<std::string, std::string>>> yandex_request_data =
            PrepareYandexRequest(text, source_lang, target_lang);

        // URL API Яндекса v2 (из документации): POST https://translate.api.cloud.yandex.net/translate/v2/translate
        std::string yandex_api_url = "translate.api.cloud.yandex.net:443";
        std::string yandex_target = "/translate/v2/translate";

        TranslationResult yandex_result = GetTranslationFromOne(
            "Яндекс.Облако",
            yandex_api_url,
            yandex_target,
            http::verb::post,
            yandex_request_data.first,
            yandex_request_data.second
        );
        results.push_back(yandex_result);

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при запросе к Яндекс.Облако: " << e.what() << std::endl;
        results.push_back(TranslationResult("Яндекс.Облако", "", false, e.what()));
    }
    // ------ В будущем тут будут запросы ко второму и третьему переводчикам ------
    // Пока добавляем заглушки для демонстрации структуры с 3+ источниками.
    results.push_back(TranslationResult("Второй Переводчик (заглушка)", "Перевод от второго (заглушка)", true));
    results.push_back(TranslationResult("Третий Переводчик (заглушка)", "Перевод от третьего (заглушка)", true));
    
    return results; 
}


TranslationResult OnlineTranslatorsManager::GetTranslationFromOne(
    const std::string& translator_name, 
    const std::string& url,
    const std::string& target,
    http::verb method,
    const std::string& request_body,
    const std::vector<std::pair<std::string, std::string>>& headers)
{
    std::string translated_text = "Ошибка: Не удалось получить перевод";
    bool success = false;
    std::string error_message = "";

    try {
        std::string host;
        std::string port;
        size_t colon_pos = url.find(":");
        if (colon_pos == std::string::npos) {
             host = url;
             port = "443"; 
        } else {
             host = url.substr(0, colon_pos);
             port = url.substr(colon_pos + 1);
        }

        // HttpClient::SendRequest пока не поддерживает HTTPS!
        std::string response_body = http_client_.SendRequest(
            host, port, target, method, request_body, headers
        );

        // --- Разбор JSON ответа ---
        if (translator_name == "Яндекс.Облако") {
            translated_text = ParseYandexResponse(response_body);
            if (translated_text.find("Ошибка") == std::string::npos) {
                success = true;
                error_message = "";
            } else {
                success = false;
                error_message = translated_text;
                translated_text = "";
            }

        }
        // ------ В будущем тут будут вызовы Parse...Response для других переводчиков ------

    } catch (const std::exception& e) {
        error_message = e.what();
        success = false;
        translated_text = "";
    }

    return TranslationResult(translator_name, translated_text, success, error_message); 
}

// Реализация функции для подготовки запроса для API Яндекса.
// Использует API ключ из api_keys_.
std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
OnlineTranslatorsManager::PrepareYandexRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang) {
    std::stringstream body_ss;
    body_ss << "{";
    body_ss << "\"sourceLanguageCode\": \"" << source_lang << "\",";
    body_ss << "\"targetLanguageCode\": \"" << target_lang << "\",";
    body_ss << "\"texts\": [\"" << text << "\"]"; 
    body_ss << "}";

    std::string request_body = body_ss.str();

    std::vector<std::pair<std::string, std::string>> headers = {
        {"Content-Type", "application/json"}, 
        {"Authorization", "Api-Key " + api_keys_.yandex_key} 
    };

    return {request_body, headers}; 
}

std::string OnlineTranslatorsManager::ParseYandexResponse(const std::string& response_body) {
     try {
        json::value jv = json::parse(response_body);

        // Проверка: является ли верхний уровень объектом и есть ли в нем массив "translations"
        if (!jv.is_object() || !jv.as_object().count("translations") || !jv.as_object()["translations"].is_array()) {
            return "Ошибка разбора ответа Яндекса: неверная структура ответа.";
        }

        json::array translations_array = jv.as_object()["translations"].as_array(); // Получаем ссылку на массив переводов

        // Проверка: не пуст ли массив "translations"
        if (translations_array.empty()) {
            return "Ошибка разбора ответа Яндекса: массив переводов пуст.";
        }

        // Проверка: является ли первый элемент массива объектом и есть ли в нем строка "text"
        if (translations_array[0].is_object()) {
            json::object translation_obj = translations_array[0].as_object(); // <-- Получаем объект из первого элемента массива (который типа value)
            if (translation_obj.count("text") && translation_obj.at("text").is_string()) {
                // УСПЕХ: Извлекаем строку перевода из поля "text" объекта
                return std::string(translation_obj.at("text").as_string().c_str()); // <-- Исправлено здесь
            } else {
                return "Ошибка разбора ответа Яндекса: неверный формат поля 'text' в первом переводе.";
            }
        } else {
            return "Ошибка разбора ответа Яндекса: первый элемент массива переводов не является объектом.";
        }

    } catch (const boost::system::system_error& e) {
        std::cerr << "Ошибка парсинга JSON ответа Яндекса: " << e.what() << std::endl;
        return "Ошибка парсинга ответа Яндекса.";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при обработке JSON ответа Яндекса (структура): " << e.what() << std::endl;
        return "Ошибка обработки ответа Яндекса (структура).";
    }
}

/*
std::vector<TranslationResult> OnlineTranslatorsManager::GetTranslations(
// ------ Звонок первому переводчику (Яндекс) ------
    try { // <-- Строка 22: Говорим: "Попробуй сделать это, но если что-то пойдет не так (Телефон крикнет ошибку), переходи к блоку catch".
// Подготавливаем, что сказать Яндексу и какие "приветы" отправить.
// !!! Это placeholder. Реальная подготовка зависит от API Яндекса !!!
        std::string yandex_url = "translate.yandex.net"; // Пример адреса
        std::string yandex_port = "443"; // Пример порта (для HTTPS)
        std::string yandex_target = "/api/v1/translate"; // Пример пути
// Пример тела запроса (JSON). Нужно смотреть документацию Яндекса!
        std::string yandex_request_body = "{ \"text\": \"" + text + "\", \"srcLang\": \"" + source_lang + "\", \"targetLang\": \"" + target_lang + "\" }";
// Пример заголовков. Нужно смотреть документацию Яндекса (API ключ)!
        std::vector<std::pair<std::string, std::string>> yandex_headers = {
            {"Content-Type", "application/json"},
// {"Authorization", "Bearer YOUR_YANDEX_API_KEY"} // Пример заголовка с API ключом
        };
        // Звоним Яндексу, отправляем запрос POST с телом и заголовками.
// Имя Управляющего для этого звонка: "Яндекс Переводчик"
        TranslationResult yandex_result = GetTranslationFromOne( // <-- Строка 39: Звоним одному переводчику, используя секретную инструкцию.
            "Яндекс Переводчик", // <-- Строка 40: Имя для коробки результата.
            yandex_url + ":" + yandex_port, // <-- Строка 41: Полный адрес для звонка.
            yandex_target, // <-- Строка 42: Куда именно на сервере.
            http::verb::post, // <-- Строка 43: Метод POST (часто используется для отправки данных).
            yandex_request_body, // <-- Строка 44: Текст сообщения (тело запроса).
            yandex_headers // <-- Строка 45: Дополнительные "приветы".
        );
        results.push_back(yandex_result); // <-- Строка 46: Кладем коробку с результатом от Яндекса в наш список.

    } catch (const std::exception& e) { // <-- Строка 48: Если Телефон крикнул ошибку (выбросил исключение) во время звонка Яндексу...
// Ловим ошибку и создаем коробку результата с информацией об ошибке.
        std::cerr << "Ошибка при запросе к Яндекс Переводчику: " << e.what() << std::endl; // <-- Строка 50: Выводим сообщение об ошибке в консоль (для отладки).
        results.push_back(TranslationResult("Яндекс Переводчик", "", false, e.what())); // <-- Строка 51: Кладем в список коробку с пометкой "не получилось" и текстом ошибки.
    }

    разбор JSON ответа
try { // <-- Строка 165: Пробуем разобрать JSON. Это тоже может вызвать ошибку, если ответ - неверный JSON.

            // Создаем парсер JSON.
            json::parser p; // <-- Строка 168: Инструмент для разбора JSON.

            // Передаем парсеру текст ответа.
            p.write(response_body, ec); // <-- Строка 171: "Сканируем" текст ответа (response_body) с помощью парсера. Если ошибка, она в ec.
            if (ec) { // <-- Строка 172: Проверяем, была ли ошибка при "сканировании" JSON текста.
                 // Если ошибка при разборе самого JSON (неправильный формат ответа).
                 std::stringstream ss; // <-- Строка 174: Помогает собрать текст ошибки.
                 ss << "Ошибка разбора JSON ответа от " << translator_name << ": " << ec.message(); // <-- Строка 175: Собираем сообщение: "Ошибка чтения записной книжки от ...: ..."
                 throw std::runtime_error(ss.str()); // <-- Строка 176: Кричим "ОШИБКА разбора JSON!"
            }

            // Получаем разобранный JSON объект (это как открытая записная книжка).
            json::value jv = p.release(); // <-- Строка 180: Получаем результат разбора. Это может быть JSON объект (для ответов API), массив, строка, число и т.д.

            // --- Шаг: Извлечение перевода из JSON ---
            // !!! ЛОГИКА ИЗВЛЕЧЕНИЯ ЗАВИСИТ ОТ ФОРМАТА ОТВЕТА КАЖДОГО API !!!
            // Это самый НЕспецифичный пример. В реальном коде тут будет логика для Яндекса, Azure и др.
            // Например, для Яндекс.Переводчика (примерный ответ {"translations": [{"text": "..."}]})
            // Нужно найти путь к переведенному тексту в этой "записной книжке".
            if (jv.is_object()) { // <-- Строка 189: Если результат разбора - это JSON объект (как обычно бывает с ответами API)...
                json::object& obj = jv.as_object(); // <-- Строка 190: Получаем доступ к содержимому объекта.

                 // !!! ЭТО ПРИМЕРНЫЙ КОД ДЛЯ ИЗВЛЕЧЕНИЯ ПЕРЕВОДА. !!!
                 // !!! НУЖНО АДАПТИРОВАТЬ ДЛЯ КАЖДОГО API СОГЛАСНО ИХ ДОКУМЕНТАЦИИ !!!

                 // Например, для Яндекс: ответ {"translations": [{"text": "Hello"}]}
                 // Надо искать "translations", потом первый элемент массива, потом "text".
                 if (obj.count("translations") && obj["translations"].is_array() && !obj["translations"].as_array().empty()) { // <-- Строка 197: Проверяем, есть ли ключ "translations", является ли его значение массивом и не пустой ли массив.
                     json::array& translations_array = obj["translations"].as_array(); // <-- Строка 198: Получаем доступ к массиву переводов.
                     if (translations_array[0].is_object() && translations_array[0].as_object().count("text") && translations_array[0].as_object()["text"].is_string()) { // <-- Строка 199: Проверяем, что первый элемент массива - объект, у него есть ключ "text", и его значение - строка.
                         translated_text = translations_array[0].as_object()["text"].as_string(); // <-- Строка 200: УРА! Нашли переведенный текст! Копируем его.
                         success = true; // <-- Строка 201: Получилось! Отмечаем успех.
                     } else { // <-- Строка 202: Не смогли найти переведенный текст по ожидаемому пути.
                          error_message = "Не удалось найти переведенный текст в ответе (неверный формат)"; // <-- Строка 203: Ошибка: "Не могу найти перевод в записной книжке".
                     }
                 } else { // <-- Строка 205: Нет ключа "translations" или он не в ожидаемом формате.
                     error_message = "Не удалось найти ключ 'translations' в ответе"; // <-- Строка 206: Ошибка: "Нет раздела 'translations' в записной книжке".
                 }

                 // Тут можно добавить логику для разбора других форматов ответа или ошибок API внутри JSON.
                 // Например, некоторые API возвращают ошибку в виде специального JSON объекта.

            } else { // <-- Строка 211: Если результат разбора - не JSON объект.
                 error_message = "Ответ сервера не является ожидаемым JSON объектом."; // <-- Строка 212: Ошибка: "Записная книжка не того типа (не объект)".
            }

        } catch (const std::exception& e) { // <-- Строка 216: Ловим любые другие ошибки, которые могли произойти во время разбора или извлечения из JSON.
            // Ошибка во время разбора JSON или извлечения данных.
            std::stringstream ss;
            ss << "Исключение при разборе/извлечении JSON от " << translator_name << ": " << e.what();
            error_message = ss.str(); // Сохраняем текст ошибки.
        }



// Аналогично для Azure...
std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
OnlineTranslatorsManager::PrepareAzureRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang) {
     // !!! ЗДЕСЬ НУЖНО РЕАЛИЗОВАТЬ ЛОГИКУ ПОДГОТОВКИ ЗАПРОСА ДЛЯ API AZURE !!!
     // Вернуть {тело_запроса_как_строка, список_заголовков}.
     // Пример:
     std::string body = "[{\"Text\":\"" + text + "\"}]";
     std::vector<std::pair<std::string, std::string>> headers = {
         {"Ocp-Apim-Subscription-Key", "YOUR_AZURE_API_KEY"}, // СЮДА НУЖЕН ТВОЙ API КЛЮЧ AZURE!
         {"Ocp-Apim-Subscription-Region", "YOUR_AZURE_REGION"}, // СЮДА НУЖЕН ТВОЙ РЕГИОН AZURE!
         {"Content-Type", "application/json"}
     };
     return {body, headers};
}

std::string OnlineTranslatorsManager::ParseAzureResponse(const std::string& response_body) {
     // !!! ЗДЕСЬ НУЖНО РЕАЛИЗОВАТЬ ЛОГИКУ РАЗБОРА JSON ОТВЕТА ОТ API AZURE !!!
      try {
         json::value jv = json::parse(response_body); // Пример парсинга JSON
         // Пример: ответ [{"translations":[{"text":"Hello"}]}]
         if (jv.is_array() && !jv.as_array().empty()) {
              if (jv.as_array()[0].is_object() && jv.as_array()[0].as_object().count("translations") && jv.as_array()[0].as_object()["translations"].is_array() && !jv.as_array()[0].as_object()["translations"].as_array().empty()) {
                   if (jv.as_array()[0].as_object()["translations"].as_array()[0].is_object() && jv.as_array()[0].as_object()["translations"].as_array()[0].as_object().count("text")) {
                         return jv.as_array()[0].as_object()["translations"].as_array()[0].as_object()["text"].as_string();
                   }
              }
         }
         return "Не удалось найти перевод в ответе Azure."; // Или выбросить исключение
     } catch (const std::exception& e) {
         std::cerr << "Ошибка разбора JSON ответа Azure: " << e.what() << std::endl;
         return "Ошибка разбора ответа Azure."; // Или выбросить исключение
     }
}

// И для третьего переводчика...
std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
OnlineTranslatorsManager::PrepareThirdRequest(const std::string& text, const std::string& source_lang, const std::string& target_lang) {
    // !!! ЗДЕСЬ НУЖНО РЕАЛИЗОВАТЬ ЛОГИКУ ПОДГОТОВКИ ЗАПРОСА ДЛЯ ТРЕТЬЕГО API !!!
    // Вернуть {тело_запроса_как_строка, список_заголовков}.
     // Пример для Google Cloud Translation Basic (API v2):
     std::string body = "q=" + text + "&source=" + source_lang + "&target=" + target_lang;
     std::vector<std::pair<std::string, std::string>> headers = {
          {"Content-Type", "application/x-www-form-urlencoded"},
          // {"X-Goog-Api-Key", "YOUR_GOOGLE_API_KEY"} // СЮДА НУЖЕН ТВОЙ API КЛЮЧ GOOGLE!
     };
     return {body, headers};
}

std::string OnlineTranslatorsManager::ParseThirdResponse(const std::string& response_body) {
    // !!! ЗДЕСЬ НУЖНО РЕАЛИЗОВАТЬ ЛОГИКУ РАЗБОРА JSON ОТВЕТА ОТ ТРЕТЬЕГО API !!!
     // Вернуть переведенный текст как строку.
     // Пример для Google Cloud Translation Basic (API v2): ответ {"data":{"translations":[{"translatedText":"Hello"}]}}
     try {
         json::value jv = json::parse(response_body); // Пример парсинга JSON
         if (jv.is_object() && jv.as_object().count("data") && jv.as_object()["data"].is_object() && jv.as_object()["data"].as_object().count("translations") && jv.as_object()["data"].as_object()["translations"].is_array() && !jv.as_object()["data"].as_object()["translations"].as_array().empty()) {
              if (jv.as_object()["data"].as_object()["translations"].as_array()[0].is_object() && jv.as_object()["data"].as_object()["translations"].as_array()[0].as_object().count("translatedText")) {
                   return jv.as_object()["data"].as_object()["translations"].as_array()[0].as_object()["translatedText"].as_string();
              }
         }
         return "Не удалось найти перевод в ответе третьего."; // Или выбросить исключение
     } catch (const std::exception& e) {
         std::cerr << "Ошибка разбора JSON ответа третьего: " << e.what() << std::endl;
         return "Ошибка разбора ответа третьего."; // Или выбросить исключение
     }
}
*/



/*
#include "http_client.hpp"
#include <json.hpp>
#include <stdexcept>

using json = nlohmann::json;

OnlineTranslators::OnlineTranslators() : httpClient_(std::make_unique<HttpClient>()) {}

bool OnlineTranslators::isNetworkAvailable() const {
    // Простая проверка доступности сети через пинг google.com
    try {
        auto response = httpClient_->get("google.com", "80", "/");
        return !response.empty();
    }
    catch(...) {
        return false;
    }
}

std::string OnlineTranslators::translateGoogle(const std::string& text, 
                                             const std::string& fromLang, 
                                             const std::string& toLang) {
    if (!isNetworkAvailable()) {
        throw std::runtime_error("Network is not available");
    }
    
    // Формируем URL для запроса
    std::string target = "/language/translate/v2?key=" + googleApiKey_ + 
                        "&q=" + text + 
                        "&source=" + fromLang + 
                        "&target=" + toLang;
    
    // Выполняем GET запрос
    auto response = httpClient_->get(googleUrl_, "443", target);
    
    // Парсим JSON ответ
    auto jsonResponse = json::parse(response);
    return jsonResponse["data"]["translations"][0]["translatedText"].get<std::string>();
}

std::string OnlineTranslators::translateYandex(const std::string& text, 
                                             const std::string& fromLang, 
                                             const std::string& toLang) {
    if (!isNetworkAvailable()) {
        throw std::runtime_error("Network is not available");
    }
    
    // Формируем тело запроса в формате JSON
    json requestBody;
    requestBody["sourceLanguageCode"] = fromLang;
    requestBody["targetLanguageCode"] = toLang;
    requestBody["texts"] = {text};
    
    // Формируем заголовки
    std::string target = "/translate/v2/translate";
    
    // Выполняем POST запрос
    auto response = httpClient_->post(yandexUrl_, "443", target, requestBody.dump());
    
    // Парсим JSON ответ
    auto jsonResponse = json::parse(response);
    return jsonResponse["translations"][0]["text"].get<std::string>();
}

std::string OnlineTranslators::translateDeepL(const std::string& text, 
                                           const std::string& fromLang, 
                                           const std::string& toLang) {
    if (!isNetworkAvailable()) {
        throw std::runtime_error("Network is not available");
    }
    
    // Формируем параметры запроса
    std::string target = "/v2/translate?auth_key=" + deeplApiKey_ + 
                        "&text=" + text + 
                        "&source_lang=" + fromLang + 
                        "&target_lang=" + toLang;
    
    // Выполняем POST запрос
    auto response = httpClient_->post(deeplUrl_, "443", target, "");
    
    // Парсим JSON ответ
    auto jsonResponse = json::parse(response);
    return jsonResponse["translations"][0]["text"].get<std::string>();
}
*/