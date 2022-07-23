#pragma once

// I tried using dpp, it kept crashing..

#define CURL_STATICLIB

#include <string>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <UE/structs.h>

#include <curl/curl.h>
class DiscordWebhook {
public:
    // Parameters:
    // - webhook_url: the discord HostingWebHook url
    DiscordWebhook(const char* webhook_url);
    ~DiscordWebhook();

    // Sends the specified message to the HostingWebHook.
    void send_message(const std::string& message);
private:
    CURL* curl;
};

DiscordWebhook::DiscordWebhook(const char* webhook_url) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, webhook_url);

        // Discord webhooks accept json, so we set the content-type to json data.
        curl_slist* headers = curl_slist_append(NULL, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    else {
        std::cerr << "Error: curl_easy_init() returned NULL pointer" << std::endl;
    }
}

DiscordWebhook::~DiscordWebhook() {
    curl_global_cleanup();
    curl_easy_cleanup(curl);
}

void DiscordWebhook::send_message(const std::string& message) {
    // The POST json data must be in this format:
    // {
    //      "content": "<MESSAGE HERE>"
    // }
    std::string json = "{\"content\": \"" + message + "\"}";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
}


namespace Information 
{
	constexpr const char* HostingWebHook =																		    ("https://discord.com/api/webhooks/1000234185887318046/jXE3YjSpLVW2QZjGF5EK4lUFGgNcJoc1M0oK38XmIt0AFTdhtUkvL3z8iSxFpAahzcYe");
    constexpr const char* LogWebHook =                                                                              ("https://discord.com/api/webhooks/1000234422739665048/UFQl0aQpY02rHWUP3Dzq9Zfc016GCSC6aI5auqxhMaFMtX3e8rQfaNSgXz0FyJVyV8ve");
}

static DiscordWebhook HostingWebHook(Information::HostingWebHook);
static DiscordWebhook LogWebHook(Information::LogWebHook);

void SendDiscordStart()
{
    HostingWebHook.send_message(std::format("Servers are up on version: {}!", FN_Version));
}

void SendDiscordEnd()
{
    HostingWebHook.send_message(std::format("Servers are down on version: {}.", FN_Version));
}