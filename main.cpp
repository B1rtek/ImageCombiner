#include <iostream>
#include <SFML/Graphics.hpp>
#include <utility>

class Command {
    std::string baseInputPath, format;
    int width{}, height{};
    bool valid, gif;

public:
    Command() {
        this->valid = false;
    }

    Command(std::string baseInputPath, int width, int height, std::string format, bool gif) {
        this->baseInputPath = std::move(baseInputPath);
        this->width = width;
        this->height = height;
        this->format = std::move(format);
        this->valid = true;
        this->gif = gif;
    }

    std::string getFirstFilePath() const noexcept {
        return getNthFile(1);
    }

    std::string getNthFile(int n) const {
        return this->baseInputPath + std::to_string(n) + "." + this->format;
    }

    std::string getFileXY(int x, int y, int offset) const noexcept {
        return getNthFile((y * this->width + x + offset) % (this->width * this->height) + 1);
    }

    std::string getOutputFilename(int number = 0) const noexcept {
        std::string num = number == 0 ? "" : std::to_string(number);
        return this->baseInputPath + "Combined" + num + "." + this->format;
    }

    std::string getOutputFilenameFormat() {
        return this->baseInputPath + "Combined%d." + this->format;
    }

    std::string getOutputGifName() {
        return this->baseInputPath + "CombinedGIF.gif";
    }

    bool isValid() const noexcept {
        return valid;
    }

    bool isGif() const noexcept {
        return gif;
    }

    int getWidth() const noexcept {
        return width;
    }

    int getHeight() const noexcept {
        return height;
    }
};

Command processArgs(int argc, char **argv) {
    if (argc < 4) {
        return {};
    }
    std::string baseInputPath = argv[1];
    int width = std::stoi(argv[2]), height = std::stoi(argv[3]);
    std::string format = "png";
    bool gif = false;
    if (argc > 4) {
        int current = 4;
        while (current < argc) {
            std::string arg = argv[current];
            if (arg == "--format") {
                format = argv[++current];
            } else if (arg == "--gif") {
                gif = true;
            }
            current++;
        }
    }
    return {baseInputPath, width, height, format, gif};
}

void combine(Command command, int imageWidth, int imageHeight, int number = 0) {
    sf::Image finalImage, inputImage;
    finalImage.create(command.getWidth() * imageWidth, command.getHeight() * imageHeight, sf::Color(0, 0, 0, 0));
    for (int y = 0; y < command.getHeight(); y++) {
        for (int x = 0; x < command.getWidth(); x++) {
            inputImage.loadFromFile(command.getFileXY(x, y, number == 0 ? 0 : number - 1));
            finalImage.copy(inputImage, x * imageWidth, y * imageHeight, sf::IntRect(0, 0, imageWidth, imageHeight),
                            true);
        }
    }
    finalImage.saveToFile(command.getOutputFilename(number));
}

int main(int argc, char **argv) {
    Command command = processArgs(argc, argv);
    if (!command.isValid()) {
        std::cout << "Usage: ImageCombiner basePath width height [--format format] [--gif]" << std::endl;
        return 0;
    }
    sf::Image inputImage, finalImage;
    inputImage.loadFromFile(command.getFirstFilePath());
    int imageWidth = inputImage.getSize().x, imageHeight = inputImage.getSize().y;
    if (!command.isGif()) {
        combine(command, imageWidth, imageHeight, 0);
    } else {
        for (int i = 1; i <= command.getWidth() * command.getHeight(); i++) {
            combine(command, imageWidth, imageHeight, i);
            std::cout << i << "/" << command.getWidth() * command.getHeight() << std::endl;
        }
        std::string ffmpegCommand = "ffmpeg -i " + command.getOutputFilenameFormat() +
                                    " -vf \"fps=15,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse\" -loop 0 " +
                                    command.getOutputGifName();
        system(ffmpegCommand.c_str());
    }
    return 0;
}
