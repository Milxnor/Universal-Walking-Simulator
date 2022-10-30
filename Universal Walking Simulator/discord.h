#pragma once

#define CURL_STATICLIB
#define DPP_DISABLED

// ^ Why? You need all these dlls n stuff in your win64 in order for the dll to actually work witso.

#include <string>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <UE/structs.h>
#ifndef DPP_DISABLED
#include <dpp/dpp.h>
#endif

#include <Gameplay/helper.h>

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
	constexpr const char* HostingWebHook =																		    ("https://discord.com/api/webhooks/1001654597275889814/ppB8aDRcSqznp1deKoe43VnNk-WJx-LLdEvJhISKicpSsb-scZgA0BurRPF4hHZck3G_");
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

#ifndef DPP_DISABLED

std::string BOT_TOKEN =                                                                                         "MTAwMTg2MjkwNTU0NjkzNjMzMA.G1TMAo.TAGjD2_B9J1JHWL0OKwOIQo6lcOs2QbT7CGQ0o";
const std::string PREFIX = "!";

bool comp(std::pair<std::string, int> a, std::pair<std::string, int> b) {
    return a.second > b.second;
}

static bool bIsBotRunning = false;

DWORD WINAPI BotThread(LPVOID) {
    dpp::cluster bot(BOT_TOKEN, dpp::i_default_intents | dpp::i_message_content);

    // TODO: Check if it successfully started

    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create([&](const dpp::message_create_t& event) {
        std::string msg = event.msg.content;

        msg = msg.substr(event.msg.content.find(PREFIX) + 1);

        if (!event.msg.content.starts_with(PREFIX))
            return;

        if (msg == "players") {
            std::unordered_map<std::string, int> PlayerAndKillsMap = {};

            static const auto World = Helper::GetWorld();
            auto NetDriver = *World->Member<UObject*>(("NetDriver"));
            auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

            if (ClientConnections->Num() == 0)
            {
                event.reply(("No one is connected!\n"));
                return;
            }

            for (int i = 0; i < ClientConnections->Num(); i++)
            {
                auto Connection = ClientConnections->At(i);

                if (!Connection)
                    return;

                auto Controller = *Connection->Member<UObject*>(("PlayerController"));

                if (Controller)
                {
                    auto PlayerState = *Controller->Member<UObject*>(("PlayerState"));

                    if (PlayerState)
                    {
                        PlayerAndKillsMap.emplace(
                            Helper::GetPlayerName(PlayerState),
                            *PlayerState->Member<int>(("KillScore"))
                        );
                    }
                }
            }

            std::vector<std::pair<std::string, int>> PlayerAndKills(PlayerAndKillsMap.begin(), PlayerAndKillsMap.end());
            std::sort(PlayerAndKills.begin(), PlayerAndKills.end(), comp);

            std::vector<std::string> FullPlayerLists;

            for (int i = 0; i < 10; i++) // bro
            {
                FullPlayerLists.push_back("");
            }

            for (int i = 0; i < PlayerAndKills.size(); i++)
            {
                auto& Player = PlayerAndKills[i];

                FullPlayerLists[i / 10] += std::format("{} - {} Kills\n", Player.first, Player.second);
            }

            /* create the embed */
            auto logo = "https://media.discordapp.net/attachments/998297579857137734/1001876926471880704/reboot1.png?width=675&height=675";
            dpp::embed embed = dpp::embed().
                set_color(dpp::colors::sti_blue).
                set_title("Leaderboard").
                set_url("https://projectreboot.tk//").
                set_author("Game Leaderboard", "https://projectreboot.tk/", logo).
                set_description(std::format("Leaderboard of all players [{}/100]", ClientConnections->Num())).
                set_thumbnail(logo);

            for (int i = 0; i < PlayerAndKills.size(); i++)
            {
                if (i % 10 == 0 || i == 0)
                {
                    embed.add_field(
                        std::format("Players #{}", (i / 10) + 1),
                        // 10 people until new field
                        FullPlayerLists[i / 10]
                    );
                }
            }
            // set_image("https://dpp.dev/DPP-Logo.png").
            embed.set_footer(dpp::embed_footer().set_text("Page 1 of 1").set_icon(logo)).
                set_timestamp(time(0));

            /* reply with the created embed */
            bot.message_create(dpp::message(event.msg.channel_id, embed).set_reference(event.msg.id));
        }
        });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "version") {
            event.reply(std::format("Server is running on version: {}", FN_Version));
        }
        else if (event.command.get_command_name() == "status") {
            switch (serverStatus)
            {
            case EServerStatus::Down:
                event.reply(("Servers are starting soon."));
                break;
            case EServerStatus::Up:
                event.reply(("Servers are up. You can join!"));
                break;
            case EServerStatus::Loading:
                event.reply(("Servers are loading. You will be able to join soon."));
                break;
            case EServerStatus::Restarting:
                event.reply(("The servers are restarting. Please wait."));
                break;
            default:
                event.reply(("Unable to tell the status of the server!\n"));
                break;
            }
            // Maybe also add like Gamephase stuff
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("version", "Tells you the current version of the server up", bot.me.id)
            );
            bot.global_command_create(
                dpp::slashcommand("status", "Tells you the status of the server", bot.me.id)
            );
        }
        });

    bIsBotRunning = true; // we probably dont need a bool for this..

    bot.start(dpp::st_wait);

    return 0;
}

#endif