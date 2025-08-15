# 📬 Kalmni — Real-Time Chat Application

Kalmni is a custom-built real-time chat application developed in **C++** with **Boost.Asio** for networking and **Dear ImGui** for the user interface.  
It follows a **Client/Server T-CP model** and supports multiple concurrent users with smooth, modern UI theming.

---

## 🚀 Features
- **Real-time messaging** between multiple clients.
- **Custom usernames** displayed in messages.
- **Smooth, modern blue UI theme** using Dear ImGui.
- **Enter key** sends messages instantly.
- **Quit button** to close the chat gracefully.
- **Developer credit** displayed in faded text in the UI.
- **Cross-platform** (Linux, Windows, macOS with minor tweaks).
- **Scalable TCP architecture** using Boost.Asio.

---

## 🛠 Technologies Used
- **C++17**
- **Boost.Asio** (TCP networking)
- **Dear ImGui** (UI rendering)
- **OpenGL & GLFW** (UI backend)
- **Multithreading** with `std::thread` and mutexes
- **Signal handling** for clean server shutdown

---

## ⚙️ Build Instructions

### Build the Server
```bash
g++ server.cpp -o server -lboost_system -pthread
```

### Build the Client
```bash
g++ client.cpp imgui/*.cpp -Iimgui -o client -lboost_system -lGL -lglfw -pthread
```

---

## 📡 Usage

### Start the Server
```bash
./server
```

### Start the Client
```bash
./client
```

1. Ensure all devices are on the **same network** or use **port forwarding** for internet use.
2. Enter the **server IP** in the client.
3. Chat in real time with connected users.

---

## 📷 Screenshots
### Client UI
![Client Login Window](screenshots/login_window.png)

![Client Chat Window](screenshots/chat_window.png)

---

## 📄 License
Programmed by **Beshoy Fomail**

---

## 📬 Contact
- **LinkedIn:** [Beshoy Fomail](https://www.linkedin.com/in/beshoy-fomail)
- **GitHub:** [beshoy-13](https://github.com/beshoy-13)
- **Email:** [fomailbeshoy@gmail.com](mailto:fomailbeshoy@gmail.com)

