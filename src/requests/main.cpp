#include "online_translators.hpp" 
#include <iostream>
#include <vector> 
#include <string> 


int main() { 
    std::string config_filepath = "api_keys.json";
    try { 

        OnlineTranslatorsManager manager(config_filepath);

        std::string text_to_translate = "Text for translate"; //Должны передаваться из граф интерфейса
        std::string source_lang = "en"; 
        std::string target_lang = "ru"; 

        // Запрос перевода
        std::vector<TranslationResult> translations = manager.GetTranslations( 
            text_to_translate, // Переданные параметры из интерфейса
            source_lang,       
            target_lang        
        );

        // Полученные переводы

        for (const auto& result : translations) {
            std::cout << "От " << result.translator_name << ": ";
            if (result.success) {
                std::cout << "\"" << result.translated_text << "\"";
            } else {
                std::cout << "Ошибка (" << result.error_message << ")";
            }
            std::cout << std::endl;
        }

    } catch (const std::exception& e) { 
        std::cerr << "Критическая ошибка программы: " << e.what() << std::endl; 
        return 1; // Возвращаем код ошибки 1
    }

    return 0; // Возвращаем код успеха 0
}