# Swap Discord Tipbot

Discord bot for Swap (https://swap.fyi), backed by a Redis database to reduce system resource and blockchain usages for small transactions.

## Authors

- Long Huynh  — main developer (longp.huynh@gmail.com)

## Compiling

- Requires libhiredis-dev libev-dev libssl-dev
```
sudo apt install libhiredis-dev libev-dev libssl-dev redis
git clone --recursive https://github.com/swap-dev/swap-discord-tipbot.git
cd swap-discord-tipbot
mkdir build
cd build
cmake ..
make
```

## Usage (Linux)
This bot should work with any monero-based cryptonote coins. Some editing of the source files may be required to change the bot's flavour text or command prefix.
```
1) Install and start Redis.
2) Install and start coin daemon and rpc-wallet. Allow daemon to fully sync.
3) Create a new discord app, bot user, bot token, and clientID: https://discordapp.com/developers/applications/
4) Move the config.json file from example_config to the same directory as the swap-discord-tipbot binaries and edit as necessary.
5) Have your bot join a discord server (replace CLIENTID in address with your clientID): https://discordapp.com/oauth2/authorize?client_id=CLIENTID&scope=bot
6) Start swap-discord-tipbot. You'll be prompted for initializing the faucet balance in the Redis database, the first time you run it.
7) In discord, type .help to see list of commands. Type .balance to test its connection to the rpc-wallet.
```
