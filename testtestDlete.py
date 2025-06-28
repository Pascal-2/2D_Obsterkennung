# Initialisierung der Variablen wie im JavaScript-Code
i = 1337
pool = list("jscu_is_not_so_secure_after_all_g00d_job")  # 'split('')' in JS wird zu list() in Python
flag_chars = []
step = 0  # Simuliert window.step

# Die Schleife läuft, solange Zeichen im Pool sind
while len(pool) > 0:
    # Berechnung von j
    # 'i || 1' in JS bedeutet 'i, wenn i nicht 0 oder null ist, sonst 1'.
    # In Python ist 0 ein Falsy-Wert, daher i oder 1.
    j = ((i if i != 0 else 1) * 16807 + step) % 2147483647

    # Berechnung des Indexes des Zeichens im Pool
    char_index = j % len(pool)

    # Füge das ausgewählte Zeichen zu flag_chars hinzu
    flag_chars.append(pool[char_index])

    # Entferne das Zeichen aus dem Pool
    pool.pop(char_index)

    # Aktualisiere i für die nächste Iteration
    i = j

    # Simuliere die Verdopplung von step
    step *= 2
    # Behandle den Fall, dass step nach der ersten Iteration 0 bleibt
    if step == 0 and len(flag_chars) > 0:
        step = 1

# Setze das generierte Passwort zusammen
password = "CTF{" + "".join(flag_chars) + "}"

# Gib das Passwort aus
print(password)