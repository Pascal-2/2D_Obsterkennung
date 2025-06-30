import re
import itertools
import subprocess
import os
from Crypto.PublicKey import RSA

def recover_key_and_decrypt(
    redacted_file: str,
    output_key: str,
    encrypted_file: str,
    plaintext_file: str,
    redacted_marker: str = '??'
):
    # Überprüfen, ob Eingabedatei existiert
    if not os.path.isfile(redacted_file):
        print(f"Fehler: Datei {redacted_file} existiert nicht.")
        return 1

    with open(redacted_file, 'r') as f:
        pem = f.read()

    # Hex-Daten und Marker im PEM extrahieren
    hex_blob = ''.join(re.findall(r'([0-9A-Fa-f]{2}|\?\?)', pem))

    # Marker suchen, Position jeweils auf zweier Bytes anpassen
    missing_positions = []
    i = 0
    while i < len(hex_blob):
        if hex_blob[i:i+2] == redacted_marker:
            missing_positions.append(i)
            i += 2
        else:
            i += 2

    num_missing = len(missing_positions)
    if num_missing == 0:
        print("Keine fehlenden Bytes erkannt.")
        return 1

    print(f"Erkannte {num_missing} fehlende Bytes, versuche Brute-Force...")

    # Brute Force über alle möglichen Kombinationen
    for combo in itertools.product(range(256), repeat=num_missing):
        candidate = list(hex_blob)
        for pos, byte in zip(missing_positions, combo):
            candidate[pos:pos+2] = f"{byte:02x}"
        candidate_hex = ''.join(candidate)

        try:
            der = bytes.fromhex(candidate_hex)
        except ValueError:
            # Ungültige Hexfolge überspringen
            continue

        try:
            key = RSA.import_key(der)
            with open(output_key, 'wb') as out:
                out.write(key.export_key('PEM'))
            print(f"Schlüssel erfolgreich mit Kombination {combo} wiederhergestellt.")
            break
        except (ValueError, IndexError):
            continue
    else:
        print("Schlüssel konnte nicht wiederhergestellt werden - Suchraum oder Methode anpassen.")
        return 2

    # OpenSSL-Aufruf zur Entschlüsselung mit Fehlerprüfung
    result = subprocess.run([
        'openssl', 'pkeyutl', '-decrypt',
        '-inkey', output_key,
        '-in', encrypted_file,
        '-out', plaintext_file
    ], capture_output=True, text=True)

    if result.returncode == 0:
        print(f"Entschlüsselung abgeschlossen, Klartext in {plaintext_file}")
    else:
        print(f"Fehler bei der Entschlüsselung:\n{result.stderr}")
        return 3

    return 0


# Beispielhafte Verwendung (Dateipfade anpassen):
if __name__ == '__main__':
    exit_code = recover_key_and_decrypt(
        redacted_file='key-redacted.pem',
        output_key='key-recovered.pem',
        encrypted_file='encrypted.txt',
        plaintext_file='plaintext.txt'
    )
    exit(exit_code)