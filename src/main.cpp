#include <Arduino.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

#define IBUTTON_PIN 2
#define LED_PIN 3

LiquidCrystal lcd(8,9,4,5,6,7);
OneWire onewire(IBUTTON_PIN);

const int max_chars=16;
const int max_rows=2;

// Hints

const char* msgs[] = {
    "Ahoj, jmenuji se Elektra Arduinovna, ale rikaji mi Sisla, protoze neumim moc cesky, zato ale umim cist \"slunecni\" prsteny. Nenasli jste nahodou nejake ?", 
    "Eeeee, Mmmmmm, Zzzzzz, Nejde to, tenhle je nejakej divnej, ten fakt precist nedokazu, zkus jinej.",
    "Tak tenhle prsten rika, ze ho mate hned vratit tatkovi a uz mu ho nebrat ! Tak sup ulicnici ... :-)",
    "Jee nasli jste prvni slunecni prsten bezva ! Zkuste najit dalsi, tenhle je nejakej divnej, rika samy nesmysly, prej ze mate projit branami vecneho kvileni a hned za nimi najit v korunach raj kosu tresnozroutu. No rikala jsem ze tenhle prsten je trochu na hlavu (nebo co to vlastne maji prsteny misto hlavy), ale snad si poradite a jestli ne tak Sisla za to nemuze :-)",
    "Ano ano! A je tu dalsi prsten vazene damy a panove, ale je asi zasifrovanej rika ze prej: 301\x01 6m, 219\x01 14m, 309\x01 29m, 40\x01 12m, 311\x01 7m, 45\x01 111m, 311\x01 34m, 38\x01 94m. Divny co ?",
    "No konecne jste nasli prsten se kterym se da normalne mluvit! Prej ze je tu zakopanej -=<* POKLAD *>=- ale nevi presne kde. Ale naucil me rymovacku, poslouchejte: \"Jedna kupa za druhou, od zapadu v rade jsou.\", dobry co ? No neni proste zabavny ?",
    "Jo! Dalsi prsten! Bezva! ... aaa pozor, tenhle se mnou nechce bavit, ze prej je to hlidaci prsten a strezi tajemstvi pokladu velkych Sacharidu. Hraje si na dulezityho pana hlidace a klic beztak schoval nekde pod rohozkou ... to zname.",
    "Tohle je Petruv hudebni prsten, prece necekate, ze displej umi zpivat ze ne ?",
    "Tohle je Andrejcin pohadkovy prsten, na pohadky mam moc kratky displej, smula co ? :-("
};

struct AddressMapEntry {
    byte msg_id;
    byte addr[8];
};

// iButton to hints mapping

const AddressMapEntry address_map[] = {
    {2, {0x16, 0x0C, 0x81, 0x06, 0x00, 0x00, 0x00, 0x07}}, // iButtons unused for the game
    {2, {0x0B, 0x3A, 0xD5, 0x3C, 0x00, 0x00, 0x00, 0x42}},
    {2, {0x16, 0xD9, 0xEC, 0x05, 0x00, 0x00, 0x00, 0x5D}},
    {7, {0x16, 0x06, 0x6F, 0x06, 0x00, 0x00, 0x00, 0xB0}},
    {8, {0x16, 0x96, 0x98, 0x05, 0x00, 0x00, 0x00, 0x7E}},

    {3, {0x16, 0xEE, 0x81, 0x07, 0x00, 0x00, 0x00, 0xCF}}, // 1
    {4, {0x16, 0xAE, 0x9B, 0x05, 0x00, 0x00, 0x00, 0x7C}}, // 2
    {5, {0x16, 0xCE, 0x88, 0x07, 0x00, 0x00, 0x00, 0x8A}}, // 3
    {6, {0x16, 0xD4, 0x82, 0x07, 0x00, 0x00, 0x00, 0xA3}}, // 4
};

byte degree[8] = {
    0b001100,
    0b010010,
    0b001100,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
};

unsigned int msg_id=0;
unsigned int msg_pos=0;

void set_msg(int id) {
    msg_id=id;
    msg_pos=0;

    lcd.clear();
}

char msg_char(int pos) {
    if ((pos >= int(strlen(msgs[msg_id]))) or (pos < 0)) {
        return ' ';
    } else {
        return msgs[msg_id][pos];
    }
}

void draw_msg() {
    for (int row=0; row<max_rows; row++) {
        lcd.setCursor(0,row);
        for (int col=0; col<max_chars; col++) {
            int pos = msg_pos + row*max_chars + col - max_chars * max_rows;
            lcd.print(msg_char(pos));
        }
    }
}

void scroll_msg() {
    msg_pos++;
    if (msg_pos >= (strlen(msgs[msg_id]) + max_chars * max_rows)) {
        msg_pos=0;
    }
}

int scan() {
    // Scan the OneWire bus
    byte addr[8];
    if (!onewire.search(addr)) {
        onewire.reset_search();
        // Serial.println("OneWire scan failed ...");
        return 0;
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        // CRC missmatch - address not read properly
        Serial.println("CRC failed ....");
        return 0;
    }

    Serial.print("Device found: ");
    for (int i=0; i<8; i++) {
        Serial.print(addr[i], HEX);
        Serial.print(":");
    }
    Serial.println();

    for (unsigned int map_id=0; map_id<sizeof(address_map)/sizeof(AddressMapEntry); map_id++) {
        bool found=true;
        for (int i=0; i<7; i++) {
            if (address_map[map_id].addr[i] != addr[i]) {
                found=false;
                break;
            }            
        }
        if (found==true) {
            Serial.print("Matching message id: ");
            Serial.println(address_map[map_id].msg_id);
            return address_map[map_id].msg_id;
        }
    }

    return 1;

}


void setup() {
    // put your setup code here, to run once:

    Serial.begin(9600);
    Serial.println("\n[iButton Hint Box v1.0]\n");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    lcd.begin(max_chars,max_rows);
    lcd.createChar(1, degree);
    set_msg(0);
}

void loop() {
    // put your main code here, to run repeatedly:
    
    draw_msg();
    delay(250);
    if ((msg_char(msg_pos) == ' ') and (msg_char(msg_pos-1) != ' ')) {
        delay(600);
    }
    scroll_msg();
    
    byte id = scan();
    if (id != 0) {
        set_msg(id);
    }

}
