#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <arpa/inet.h>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

static std::vector<std::string> chat_messages;
static std::mutex chat_mutex;
static bool running = true;

static void setBlueTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* c = style.Colors;

    c[ImGuiCol_WindowBg]        = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
    c[ImGuiCol_ChildBg]         = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
    c[ImGuiCol_FrameBg]         = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBgHovered]  = ImVec4(0.22f, 0.30f, 0.40f, 1.00f);
    c[ImGuiCol_FrameBgActive]   = ImVec4(0.20f, 0.34f, 0.52f, 1.00f);
    c[ImGuiCol_Button]          = ImVec4(0.20f, 0.40f, 0.70f, 1.00f);
    c[ImGuiCol_ButtonHovered]   = ImVec4(0.28f, 0.50f, 0.82f, 1.00f);
    c[ImGuiCol_ButtonActive]    = ImVec4(0.12f, 0.32f, 0.60f, 1.00f);
    c[ImGuiCol_Header]          = ImVec4(0.20f, 0.40f, 0.70f, 1.00f);
    c[ImGuiCol_HeaderHovered]   = ImVec4(0.28f, 0.50f, 0.82f, 1.00f);
    c[ImGuiCol_HeaderActive]    = ImVec4(0.20f, 0.40f, 0.70f, 1.00f);
    c[ImGuiCol_TitleBgActive]   = ImVec4(0.14f, 0.20f, 0.28f, 1.00f);
    c[ImGuiCol_ScrollbarBg]     = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);

    style.WindowRounding   = 10.0f;
    style.FrameRounding    = 8.0f;
    style.ScrollbarRounding= 8.0f;
    style.GrabRounding     = 8.0f;
}

static void receiver(int sock) {
    char buf[1024];
    while (running) {
        ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        std::lock_guard<std::mutex> lock(chat_mutex);
        chat_messages.push_back(std::string(buf));
    }
}

int main(int, char**) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port   = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    std::thread rx(receiver, sock);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        close(sock);
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("Kalmni Chat Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 620, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    setBlueTheme();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool username_set = false;
    char username[64] = "";
    char input[256]   = "";

    bool show = true;
    SDL_Event e;

    while (show) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) { show = false; }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (!username_set) {
            ImGui::SetNextWindowSize(ImVec2(380, 140), ImGuiCond_Always);
            ImGui::Begin("Enter Username", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            bool joinNow = ImGui::InputText("Username", username, sizeof(username),
                                            ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Spacing();
            if (ImGui::Button("Join") || (joinNow && username[0] != '\0')) {
                if (username[0] != '\0') {
                    username_set = true;
                    std::lock_guard<std::mutex> lock(chat_mutex);
                    chat_messages.push_back(std::string("• Joined as ") + username);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Quit")) { show = false; }
            ImGui::End();
        } else {
            ImGui::SetNextWindowSize(ImVec2(860, 560), ImGuiCond_Once);
            ImGui::Begin("Kalmni Chat", nullptr,
                         ImGuiWindowFlags_NoCollapse);

            ImGui::BeginChild("history", ImVec2(0, -60), true);
            {
                std::lock_guard<std::mutex> lock(chat_mutex);
                for (const auto& m : chat_messages) {
                    ImGui::TextWrapped("%s", m.c_str());
                }
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            bool sendNow = ImGui::InputText("Message", input, sizeof(input),
                                            ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (ImGui::Button("Send") || sendNow) {
                if (input[0] != '\0') {
                    std::string line = std::string(username) + ": " + input;
                    {
                        std::lock_guard<std::mutex> lock(chat_mutex);
                        chat_messages.push_back(line);
                    }
                    send(sock, line.c_str(), line.size(), 0);
                    input[0] = '\0';
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Quit")) { show = false; }

            ImGui::Separator();
            ImVec4 faded = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            faded.w = 0.35f;
            ImGui::PushStyleColor(ImGuiCol_Text, faded);
            ImGui::TextUnformatted("Programmed by Beshoy Fomail");
            ImGui::PopStyleColor();

            ImGui::End();
        }

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    running = false;
    shutdown(sock, SHUT_RDWR);
    close(sock);
    if (rx.joinable()) rx.join();

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
