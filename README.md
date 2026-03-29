# 💬 ft_irc

> A fully functional Internet Relay Chat (IRC) server written in C++98.

## 💡 About the Project
This project involves recreating a completely functional IRC server from scratch. It is designed to handle multiple concurrent client connections without crashing or blocking. By strictly adhering to the C++98 standard and utilizing non-blocking I/O multiplexing, this server provides a lightweight, robust, and real-time communication platform compatible with standard IRC clients.

## 🚀 Features
* **Multi-Client Handling:** Uses `poll()` and non-blocking sockets (`fcntl`) to manage multiple users simultaneously on a single thread.
* **Authentication:** Secure server access requiring a password upon connection.
* **Direct Messaging:** Private messaging between users (`PRIVMSG`).
* **Channel Management:** * Users can create, join, and leave channels (`JOIN`, `PART`, `QUIT`).
  * Channel Operators can manage the channel and kick malicious users (`KICK`).
  * Support for channel topics (`TOPIC`).
  * Support for inviting users to channels (`INVITE`).
* **Channel Modes (`MODE`):**
  * `i`: Set/remove Invite-only channel.
  * `t`: Set/remove the restrictions of the TOPIC command to channel operators.
  * `k`: Set/remove the channel key (password).
  * `o`: Give/take channel operator privilege.
  * `l`: Set/remove the user limit to channel.
* **Standard Client Compatibility:** fully compatible with standard IRC clients like **Irssi**, **WeeChat**, or **Netcat**.

## 🛠️ Tech Stack & Dependencies
* **Language:** C++ (C++98 Standard)
* **Build System:** Makefile
* **Core Concepts:** TCP/IP Networking, Socket Programming (`socket`, `bind`, `listen`, `accept`), I/O Multiplexing (`poll`), Object-Oriented Programming (OOP).

## ⚙️ Installation & Usage

### Prerequisites
* C++98 compliant compiler (c++, clang++, or g++)
* Make
* An IRC Client (like `irssi` or `nc` for testing)

### Compilation
Clone the repository and compile the project using the provided Makefile:

```bash
git clone https://github.com/SASSI42/ft_irc.git ft_irc
cd ft_irc
make
```

### Running the Server
The server requires a port number and a password to start:
```bash
./ircserv <port> <password>
# Example:
./ircserv 6667 my_secure_password
```

### Connecting as a Client
You can connect to the server using `irssi` from another terminal:
```bash
irssi -c 127.0.0.1 -p 6667 -w my_secure_password -n MyNickname
```

Alternatively, you can connect using `netcat` and send raw IRC commands:
```bash
nc 127.0.0.1 6667
PASS my_secure_password
NICK MyNickname
USER MyNickname 0 * :Real Name
JOIN #42Network
PRIVMSG #42Network :Hello everyone!
```

## 🧠 Architecture & Implementation Details

This server is designed using clean Object-Oriented principles to separate networking logic from IRC protocol execution:

1. **The Event Loop (`Server` class):** The core of the server runs on a `poll()` event loop. It monitors the main server socket for incoming connections (`POLLIN`) and handles new clients dynamically. All file descriptors are strictly set to `O_NONBLOCK` to ensure a single slow client cannot freeze the entire server.
2. **Client Management (`Client` class):** Each connection is wrapped in a `Client` object containing a data buffer. Because TCP streams can fragment data, the server appends incoming packets to the buffer and only parses messages when a complete `\r\n` delimiter is detected.
3. **Command Dispatching (`CommandDispatcher`):** To avoid massive `if-else` blocks, the server uses a Command Dispatcher pattern. Parsed strings are mapped to specific handler classes (`JoinCommand`, `PrivmsgCommand`, `ModeCommand`, etc.) derived from a base `Command` class.
4. **Channels & State (`Channel` class):** Channels maintain state regarding their current members, operators, invited lists, and active modes. 

## 👨‍💻 Author
* [Mohammed Sassi](https://github.com/SASSI42)
* [Ahmed Sadik](https://github.com/42charlie)
