🔥 Authorized Load Testing Framework v3
CF/WAF Bypass Multi-Threaded HTTP Brute Force Engine

Tool by Seabuhi

Bu layihə yalnız icazəli, qanuni, təhlükəsiz şəkildə yük testləri, WAF/Cloudflare bypass analizləri, login endpointlərinin stress testləri, və security hardening yoxlamaları üçün nəzərdə tutulub.

✨ Xüsusiyyətlər

⚡ Multi-Thread Brute-Force Engine (50 thread — configurable)

🛡 Cloudflare / WAF Bypass üçün random User-Agent rotation

🌍 Proxy Rotation (proxies.txt)

📡 HTTP Response interception & success-detection

🧵 Thread-safe wordlist oxuma (pthread_mutex)

🔄 Human-like delay — WAF-trigger riskini azaldır

🎨 Terminal vizual effektləri (matrix rain, pulse, banner)

🔗 Stabil cURL HTTP engine (-lcurl)

📌 İstifadə Qaydası
1. Compile (Linux / Debian / Ubuntu)
$ gcc authorized_bruteforce.c -o bruteforce -lcurl -lpthread

2. proxies.txt yaratmaq (opsional)
http://12.34.56.78:8080
socks5://11.22.33.44:1080
http://88.99.11.22:3128

3. Tool-u işə salmaq
$ ./bruteforce https://target.com/login admin rockyou.txt

--------------------------------------------------------
Usage:

$ bruteforce <login_url> <username> <wordlist_file>


Misal:

$ ./bruteforce https://site.com/auth seabuhi wordlist.txt

🎯 Necə işləyir?
Brute force threading modeli

Hər thread:

Wordlist-dən bir parola keçir

UA rotation → WAF/CF bypass

Proxy rotation → IP footprint azaltma

POST request göndərir

Cavabda "Welcome to the Dashboard" varsa → uğur sayılır

Bütün thread-lər dayandırılır

Kodun uğur hissəsi:

if (http_code == 200 && strstr(chunk.memory, "Welcome to the Dashboard")) {
    printf("\n[!!!] SUCCESS! Password found: %s\n", current_password);
    keep_running = 0;
}

🧩 Fayl Strukturu
/project
│
├── authorized_bruteforce.c
├── proxies.txt
├── wordlist.txt
└── README.md

⚙️ Konfiqurasiyalar
Parametr	İzah
NUM_THREADS	Thread sayı (default: 50)
PROXY_FILE	Proxy siyahısı
random_user_agent()	CF/WAF bypass üçün UA rotation
WriteMemoryCallback()	HTTP cavabını RAM-da saxlayır
keep_running	Thread-ləri dayandıran global flag
📘 Asılılıqlar

Ubuntu üçün asılılıqlar:

$ sudo apt install libcurl4-openssl-dev
$ sudo apt install build-essential

🛡 Qanuni Bildiriş

Bu tool yalnız aşağıdakılar üçün istifadəyə icazəlidir:

Öz serverləriniz

Icazəli pentest

Müqaviləli load test

Sizə məxsus veb tətbiqlər

İcazəsiz istifadə qanun pozuntusudur.
Developer məsuliyyət daşımır.

👑 Author

Developer: Seabuhi

GitHub:    https://github.com/seabuhi
Instagram: @seabuhi
