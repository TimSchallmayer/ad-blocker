# 🛑 Ad Blocker (Chrome Extension)

A lightweight and simple ad blocker built in C++ to make a Pi-hole like desktop programm.
This project focuses on blocking ads efficiently while staying easy to understand and modify.

---

## 🚀 Features

- Blocks ads/malware on websites  
- Lightweight and fast  
- Easy to install and use  

---

## 🧠 How It Works

The programm interfieres the packages sent to the DNS server and scans for domains on the blocklist.

**Basic process:**
1. A website loads  
2. The programm scans the domain
3. The domain is blocked, if on the blocklist

---

## 🛠 Installation

1. Download the current Release

2. Execute the programm in the background along with the blocklist in the same folder

✅ Done! The extension should now be active.

---

## ⚙️ Usage

- Once installed, the ad blocker works automatically, you just have to leave it open in the background
- No additional setup required  

---

## 📌 Limitations

- Not all ads may be blocked (currently uses a blockliste with ca. 50.000 domains)
- No user interface yet  
- Requires manual updates  

---

## 🔮 Future Improvements

- Improve ad detection logic (likely use sqlite or something)  
- Add user interface (settings menu)  
- Track blocked ads (statistics)
  
---

## 📄 License

Copyright (c) 2025 Tim Schallmayer

All rights reserved.

This project and its source code are the intellectual property of Tim Schallmayer.

No part of this repository may be used, copied, modified, merged, published, distributed, sublicensed, or sold in any form without explicit prior written permission from the copyright holder.

---

## 🙌 Acknowledgements

Built as a learning project to understand how DNS packagin ans Networking works
