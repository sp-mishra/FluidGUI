#include "webview/webview.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <CoreGraphics/CGDisplayConfiguration.h>
#include "Log.hpp"
#include "HtmlGenerator.hpp"

int main() {
  CGDirectDisplayID displayID = CGMainDisplayID();
  size_t width = CGDisplayPixelsWide(displayID);
  size_t height = CGDisplayPixelsHigh(displayID);

  std::cout << "Screen Width: " << width << ", Screen Height: " << height << std::endl;
  try {
    long count = 0;

    // Read the content of test.html
    std::ifstream file("./test.html");
    if (!file) {
      std::cerr << "Could not open the file!" << std::endl;
      return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html = buffer.str();

    webview::webview w(true, nullptr);
    w.set_title("Bind Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);

    // A binding that counts up or down and immediately returns the new value.
    w.bind("count", [&](const std::string &req) -> std::string {
      // Imagine that req is properly parsed or use your own JSON parser.
      auto direction = std::stol(req.substr(1, req.size() - 1));
      return std::to_string(count += direction);
    });

    // A binding that creates a new thread and returns the result at a later time.
    w.bind(
        "compute",
        [&](const std::string &id, const std::string &req, void * /*arg*/) {
          // Create a thread and forget about it for the sake of simplicity.
          std::thread([&, id, req] {
            // Simulate load.
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Imagine that req is properly parsed or use your own JSON parser.
            const auto *result = "42";
            w.resolve(id, 0, result);
          }).detach();
        },
        nullptr);

    w.set_html(html);
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}