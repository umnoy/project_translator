#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iostream> 
#include <stdexcept> 

#include <boost/json.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/system/system_error.hpp> 

namespace json = boost::json;
namespace sys = boost::system;

ApiKeys load_api_keys_from_file(const std::string& filepath) {
     ApiKeys keys; 

     std::ifstream file(filepath);

     if (!file.is_open()) { 
          std::stringstream ss;
          
          ss << "CONFIGURATION ERROR: Failed to open configuration file '" << filepath << "'. Ensure the file exists and is accessible.";
          throw std::runtime_error(ss.str());
     }

     std::stringstream buffer; 
     buffer << file.rdbuf(); 
     file.close();

     std::string json_content = buffer.str();

     sys::error_code ec; 
     json::value jv = json::parse(json_content, ec); 

     if (ec) { 
          std::stringstream ss;
          ss << "CONFIGURATION ERROR: JSON parsing error in file '" << filepath << "': " << ec.message();
          throw std::runtime_error(ss.str());
     }

     try {
          json::object obj = jv.as_object(); 

          if (obj.count("yandex_translate") && obj.at("yandex_translate").is_object()) {
               json::object yandex_obj = obj.at("yandex_translate").as_object();
               if (yandex_obj.count("api_key") && yandex_obj.at("api_key").is_string()) {
                    keys.yandex_key = std::string(yandex_obj.at("api_key").as_string().c_str());
               } else {
                    std::cerr << "Warning: The key 'yandex_translate.api_key' was not found or has an incorrect format in the file '" << filepath << "'." << std::endl;
               }
          } else {
               std::cerr << "Warning: The 'yandex_translate' section was not found in the file '" << filepath << "'." << std::endl;
          }
/*
        // Пример для Azure: ищем "azure_translator" -> "subscription_key" и "region"
        if (obj.count("azure_translator") && obj.at("azure_translator").is_object()) {
             json::object azure_obj = obj.at("azure_translator").as_object();
             if (azure_obj.count("subscription_key") && azure_obj.at("subscription_key").is_string()) {
                  keys.azure_key = std::string(azure_obj.at("subscription_key").as_string()); // <-- ИСПРАВЛЕНО
             } else {
                  std::cerr << "Предупреждение: Ключ 'azure_translator.subscription_key' не найден или имеет неверный формат в файле '" << filepath << "'." << std::endl;
             }
             if (azure_obj.count("region") && azure_obj.at("region").is_string()) {
                  keys.azure_region = std::string(azure_obj.at("region").as_string()); // <-- ИСПРАВЛЕНО
             } else {
                  std::cerr << "Предупреждение: Ключ 'azure_translator.region' не найден или имеет неверный формат в файле '" << filepath << "'." << std::endl;
             }
        } else {
             std::cerr << "Предупреждение: Раздел 'azure_translator' не найден в файле '" << filepath << "'." << std::endl;
        }

        // Пример для третьего сервиса: ищем "third_translator" -> "api_key"
        if (obj.count("third_translator") && obj.at("third_translator").is_object()) {
            json::object third_obj = obj.at("third_translator").as_object();
            if (third_obj.count("api_key") && third_obj.at("api_key").is_string()) {
                keys.third_key = std::string(third_obj.at("api_key").as_string()); // <-- ИСПРАВЛЕНО
            } else {
                 std::cerr << "Предупреждение: Ключ 'third_translator.api_key' не найден или имеет неверный формат в файле '" << filepath << "'." << std::endl;
            }
        } else {
             std::cerr << "Предупреждение: Раздел 'third_translator' не найден в файле '" << filepath << "'." << std::endl;
        }
*/

     } catch (const std::exception& e) { // Ловим другие ошибки при извлечении данных (например, если at() выкинул исключение)
          std::stringstream ss;
          ss << "CONFIGURATION ERROR: Error extracting data from a JSON file '" << filepath << "': " << e.what();
          throw std::runtime_error(ss.str());
     }

     return keys; // Возвращаем заполненную структуру с ключами.
}