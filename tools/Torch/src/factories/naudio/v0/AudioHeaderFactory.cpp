#include "AudioHeaderFactory.h"

#include <vector>
#include "Companion.h"
#include "AIFCDecode.h"
#include "spdlog/spdlog.h"
#include <factories/naudio/v1/AudioConverter.h>

/*
ExportResult AudioAIFCExporter::Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) {

    auto samples = AudioManager::Instance->get_samples();

    int temp = 0;
    for(auto& sample : samples){
        std::string dpath = Companion::Instance->GetOutputPath() + "/" + (*replacement);
        if(!exists(fs::path(dpath).parent_path())){
            create_directories(fs::path(dpath).parent_path());
        }
        std::ofstream file(dpath + "_bank_" + std::to_string(++temp) + ".aiff", std::ios::binary);

        LUS::BinaryWriter aifc = LUS::BinaryWriter();
        AudioConverter::SampleV0ToAIFC(sample, aifc);
        
        LUS::BinaryWriter aiff = LUS::BinaryWriter();
        write_aiff(aifc.ToVector(), aiff);
        aifc.Close();
        aiff.Finish(file);
        file.close();
        // SPDLOG_INFO("Exported {}", dpath + "_bank_" + std::to_string(temp) + ".aiff");

        SPDLOG_INFO("sample_{}:", temp);
        SPDLOG_INFO("  type: NAUDIO:V0:SAMPLE");
        SPDLOG_INFO("  id: {}\n", temp);
    }

    return std::nullopt;
}
*/

std::optional<std::shared_ptr<IParsedData>> AudioHeaderFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& data) {
    AudioManager::Instance->initialize(buffer, data);
    return std::make_shared<IParsedData>();
}