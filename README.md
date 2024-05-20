# ESP8266 MQTT Traffic Light Controller

Dette prosjektet demonstrerer hvordan du bruker en ESP8266 mikrokontroller til å styre et sett med trafikklys og lydmodulen ved hjelp av MQTT-meldinger.

## Funksjoner

- **WiFi-tilkobling**
- **NTP-tidsynkronisering**
- **MQTT-kommunikasjon**
- **Lys- og lydkontroll**

## Maskinvarekrav

- ESP8266 mikrokontroller (f.eks. NodeMCU)
- Trafikklys med rødt, grønt, og gult lys (LED)
- Lydmodul (f.eks. buzzer)

## Oppsett

1. **Installer nødvendige biblioteker**:
   - ESP8266WiFi
   - PubSubClient
   - LittleFS
   - BearSSL

2. **Last opp SSL-sertifikater til ESP8266**:
   - Bruk `certs-from-mozilla.py` til å hente SSL-sertifikater og last opp til ESP8266 med LittleFS.

3. **Last opp koden til ESP8266**:
   - Bruk Arduino IDE eller annen kompatibel utviklingsmiljø.

## Bruk

### MQTT-emner og kommandoer

- **Emne for lyskontroll**: `lys`
  - **Kommandoer**:
    - `R`: Tenn rødt lys
    - `G`: Tenn grønt lys
    - `Y`: Tenn gult lys
    - Kombinasjoner som `RG`, `RY`, `GY`, `RGY` for å tenne flere lys samtidig
    - `X`: Slukk alle lys

- **Emne for lydkontroll**: `lyd`
  - **Kommandoer**:
    - `1`: Aktiver lyd
    - `0`: Deaktiver lyd (eller en hvilken som helst annen verdi enn `1`)

### Eksempel

Send en MQTT-melding med innholdet `RG` til emnet `lys` for å tenne rødt og grønt lys samtidig. Send `X` for å slukke alle lysene.

## Lisens

Dette prosjektet er lisensiert under [MIT-lisensen](LICENSE).
