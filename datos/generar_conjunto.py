# genera pequenio (100), mediano (1000) y grande (5000)
# la mayoria de usuarios tiene fotos, mensajes y solicitudes

import os
import random

NOMBRES = [
    "alejandro", "andrea", "andres", "angela", "camila", "carolina", "catalina",
    "daniel", "david", "diego", "elena", "esteban", "felipe", "gabriel", "isabella",
    "juan", "julian", "laura", "lucia", "luisa", "manuela", "maria", "mateo",
    "natalia", "nicolas", "paula", "santiago", "sebastian", "sofia", "valentina",
    "valeria", "vanessa", "oscar", "kevin", "miguel", "sara", "tatiana", "carlos",
]

APELLIDOS = [
    "restrepo", "gomez", "lopez", "martinez", "rodriguez", "garcia", "hernandez",
    "ramirez", "torres", "diaz", "munoz", "ruiz", "jimenez", "moreno", "alvarez",
    "romero", "navarro", "castro", "ortega", "delgado", "pena", "vargas", "rios",
    "cardona", "osorio", "mejia", "betancur", "arango", "zapata", "quintero",
]

CIUDADES = [
    "Medellin", "Bogota", "Cali", "Barranquilla", "Cartagena", "Pereira",
    "Manizales", "Bucaramanga", "Armenia", "Envigado",
]

FRASES = [
    "listo para el parcial de estructuras?",
    "vamos por un cafe despues de clase",
    "me pasas los apuntes de informatica?",
    "quedamos en la biblioteca a las 3",
    "ya subieron las notas del quiz",
    "feliz cumple, te mando abrazo",
    "esta lloviendo feo por laureles",
    "te vi en el metro pero no te alcance",
    "manana hay reunion del proyecto",
    "me gusto mucho tu foto del viaje",
    "cuando vamos a cartagena otra vez?",
    "trae el computador que hay que practicar",
    "no olvides entregar el informe",
    "hoy no alcanzo, manana te escribo",
    "gracias por la ayuda con el codigo",
    "ese algoritmo ya me tiene loco",
    "vamos al partido el domingo?",
    "te deje un mensaje en el grupo",
    "que haces este finde?",
    "ya arreglamos lo del commit",
    "pasa por la cafetera cuando puedas",
    "me salio un error raro en qt",
    "listo, ya acepte la solicitud",
    "mira las fotos del album nuevo",
    "nos vemos en el bloque 19",
    "estudiaste para el oral?",
    "buenas, todo bien por alla?",
    "dale, yo te ayudo con esa parte",
    "hoy toca laboratorio a las 10",
    "suerte en la sustentacion",
    "sale un almuerzo por el centro?",
    "ya termine la tarea de poo",
    "me prestas el cargador un rato",
    "que tal te fue en el parcial?",
    "estoy en la fila de la fotocopiadora",
]

ALBUMES = [
    "viaje", "parciales", "familia", "mascotas", "udea", "finde", "deportes",
    "comida", "amigos", "campus", "salida", "vacaciones",
]


def fecha_unica(seg_base):
    # arma AAAAMMDDhhmmss sin repetir segundo a segundo
    total = 1_700_000_000 + seg_base
    dias = total // 86400
    resto = total % 86400
    # calendario simple desde 2024-01-01
    anio = 2024
    mes = 1
    dia = 1 + dias
    largos = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
    while True:
        bisiesto = (anio % 4 == 0 and (anio % 100 != 0 or anio % 400 == 0))
        dias_mes = 29 if (mes == 2 and bisiesto) else largos[mes - 1]
        if dia <= dias_mes:
            break
        dia -= dias_mes
        mes += 1
        if mes > 12:
            mes = 1
            anio += 1
            if anio > 2026:
                anio = 2026
                mes = 12
                dia = min(dia, 28)
                break
    hh = resto // 3600
    mm = (resto % 3600) // 60
    ss = resto % 60
    return f"{anio:04d}{mes:02d}{dia:02d}{hh:02d}{mm:02d}{ss:02d}"


def generar(n, carpeta, semilla):
    random.seed(semilla)
    os.makedirs(carpeta, exist_ok=True)

    usados = []
    for i in range(n):
        usados.append(
            f"{NOMBRES[i % len(NOMBRES)]}_{APELLIDOS[(i * 7) % len(APELLIDOS)]}{i:03d}"
        )
    usados.sort()
    demo = usados[0]

    lineas_usuarios = []
    for i in range(n):
        nacimiento = (
            f"{1988 + (i % 18):04d}{(i % 12) + 1:02d}{(i % 27) + 1:02d}"
            f"{(i % 20) + 1:02d}{i % 50:02d}{i % 55:02d}"
        )
        inscripcion = (
            f"202{4 + (i % 2)}{(i % 12) + 1:02d}{(i % 27) + 1:02d}"
            f"{8 + (i % 10):02d}{i % 40:02d}{i % 50:02d}"
        )
        correo = f"{usados[i]}@udea.edu.co"
        lineas_usuarios.append(
            f"{usados[i]}|{nacimiento}|{correo}|clave123|"
            f"{CIUDADES[i % len(CIUDADES)]}|Colombia|{inscripcion}"
        )

    amigos = [[False] * n for _ in range(n)]

    def marcar(a, b):
        if a == b:
            return
        if a > b:
            a, b = b, a
        amigos[a][b] = True
        amigos[b][a] = True

    # clique para el sorteo de fotos del demo
    tope = min(24, n - 1)
    if tope >= 2:
        for a in range(tope):
            for b in range(a + 1, tope + 1):
                marcar(a, b)

    if n > 45:
        for a in range(25, min(34, n - 1) + 1):
            marcar(0, a)
            for b in range(1, min(15, tope) + 1):
                marcar(a, b)
        for a in range(35, min(44, n - 1) + 1):
            marcar(0, a)
            for b in range(1, min(5, tope) + 1):
                marcar(a, b)

    if n > 120:
        for a in range(100, min(119, n - 1) + 1):
            cuantos = 3 + ((a - 100) % 3)
            for b in range(1, min(cuantos, tope) + 1):
                marcar(a, b)
    elif n > 50:
        for a in range(45, min(54, n - 1) + 1):
            for b in range(1, min(4, tope) + 1):
                marcar(a, b)

    # casi todos con al menos 2-4 amigos
    for u in range(n):
        cuantos = 2 + (u % 3)
        for _ in range(cuantos):
            otro = random.randint(0, n - 1)
            marcar(u, otro)

    lineas_amistad = []
    for a in range(n):
        for b in range(a + 1, n):
            if amigos[a][b]:
                lineas_amistad.append(f"{a}|{b}")

    def lista_amigos(u):
        return [i for i in range(n) if amigos[u][i]]

    claves_sol = set()
    lineas_sol = []

    def agregar_sol(emisor, receptor):
        if emisor == receptor or amigos[emisor][receptor]:
            return False
        ida = f"{emisor}-{receptor}"
        vuelta = f"{receptor}-{emisor}"
        if ida in claves_sol or vuelta in claves_sol:
            return False
        claves_sol.add(ida)
        lineas_sol.append(f"{emisor}|{receptor}")
        return True

    # solicitudes hacia el demo
    candidatos = list(range(1, n))
    if len(candidatos) >= 10:
        for emisor in random.sample(candidatos, min(15, len(candidatos))):
            agregar_sol(emisor, 0)
        for receptor in random.sample(candidatos, min(10, len(candidatos))):
            agregar_sol(0, receptor)

    # ~85% de usuarios manda o recibe al menos una solicitud
    tope_sol = int(n * 0.85)
    for u in range(tope_sol):
        intentos = 0
        while intentos < 12:
            otro = random.randint(0, n - 1)
            if agregar_sol(u, otro) or agregar_sol(otro, u):
                break
            intentos += 1

    claves_msg = set()
    lineas_msg = []
    reloj = 0

    def agregar_msg(emisor, receptor, texto):
        nonlocal reloj
        if emisor == receptor or not texto:
            return False
        reloj += 1 + (emisor % 3)
        fecha14 = fecha_unica(reloj)
        cuerpo = texto
        if len(cuerpo.encode("utf-8")) > 500:
            cuerpo = cuerpo[:200]
        clave = f"{emisor}|{receptor}|{fecha14}|{cuerpo}"
        if clave in claves_msg:
            return False
        claves_msg.add(clave)
        largo = len(cuerpo.encode("utf-8"))
        lineas_msg.append(f"{emisor}|{receptor}|{fecha14}|{largo}|{cuerpo}")
        return True

    # mensajes al demo repartidos en >6 semanas (para el video)
    amigos_demo = lista_amigos(0) or list(range(1, min(11, n)))
    for k in range(30):
        texto = FRASES[k % len(FRASES)]
        if k % 5 == 0:
            texto = f"{texto} ({usados[amigos_demo[k % len(amigos_demo)]]})"
        agregar_msg(amigos_demo[k % len(amigos_demo)], 0, texto)

    # casi todos envian y reciben algo
    for u in range(n):
        amigos_u = lista_amigos(u)
        if not amigos_u:
            amigos_u = [(u + 1) % n]
        # 1 o 2 mensajes salientes
        for t in range(1 + (u % 2)):
            dest = amigos_u[(u + t) % len(amigos_u)]
            frase = FRASES[(u + t * 11) % len(FRASES)]
            agregar_msg(u, dest, f"{frase} [{usados[u]}]")
        # 1 entrante de un amigo
        origen = amigos_u[(u * 3) % len(amigos_u)]
        frase2 = FRASES[(u * 5 + 3) % len(FRASES)]
        agregar_msg(origen, u, f"{frase2} para {usados[u]}")

    lineas_msg.sort(key=lambda L: L.split("|")[2])

    lineas_media = []
    lineas_cambios = []
    id_foto = 0

    def agregar_album(usuario, nombre, privacidad, cantidad, likes_base):
        nonlocal id_foto
        lineas_media.append(f"A|{usuario}|{nombre}|{privacidad}")
        for f in range(1, cantidad + 1):
            fecha = (
                f"2026{7 + (f % 2):02d}{(f % 27) + 1:02d}"
                f"{10 + (usuario % 8):02d}{f:02d}{(f * 2 + usuario % 50) % 60:02d}"
            )
            archivo = f"{nombre}_{f}"
            lineas_media.append(f"F|{fecha}|{1200 + f * 37 + usuario}|{archivo}")
            if likes_base + f > 0:
                lineas_cambios.append(f"K|{id_foto}|{likes_base + f}")
            id_foto += 1

    # demo del perfil: usuario 1 con 2 albumes de 5 + uno privado
    if n > 1:
        agregar_album(1, "viaje_cartagena", "A", 5, 2)
        agregar_album(1, "campus", "P", 5, 1)
        agregar_album(1, "privado", "R", 2, 0)

    # ~90% de usuarios con al menos un album y fotos
    tope_fotos = int(n * 0.90)
    for u in range(n):
        if u == 1:
            continue
        if u >= tope_fotos:
            continue
        cuantos_alb = 1 + (u % 2)
        for a in range(cuantos_alb):
            nom = f"{ALBUMES[(u + a) % len(ALBUMES)]}_{u}"
            priv = "A" if (u + a) % 3 else "P"
            fotos = 1 + ((u + a) % 3)
            agregar_album(u, nom, priv, fotos, (u % 4))

    leidos = 10
    lineas_cambios = [f"L|0|{leidos}"] + lineas_cambios

    def escribir(nombre, lineas):
        with open(os.path.join(carpeta, nombre), "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(lineas))
            if lineas:
                f.write("\n")

    assert len(lineas_usuarios) == len(set(lineas_usuarios))
    assert len(lineas_amistad) == len(set(lineas_amistad))
    assert len(lineas_sol) == len(set(lineas_sol))
    assert len(lineas_msg) == len(set(lineas_msg))

    escribir("usuarios.txt", lineas_usuarios)
    escribir("multimedia.txt", lineas_media)
    escribir("amistades.txt", lineas_amistad)
    escribir("solicitudes.txt", lineas_sol)
    escribir("mensajes.txt", lineas_msg)
    escribir("cambios.txt", lineas_cambios)

    # conteos rapidos de cobertura
    con_foto = set()
    for L in lineas_media:
        if L.startswith("A|"):
            con_foto.add(int(L.split("|")[1]))
    con_msg = set()
    for L in lineas_msg:
        p = L.split("|")
        con_msg.add(int(p[0]))
        con_msg.add(int(p[1]))
    con_sol = set()
    for L in lineas_sol:
        p = L.split("|")
        con_sol.add(int(p[0]))
        con_sol.add(int(p[1]))

    print(f"=== {carpeta} ===")
    print("usuarios", len(lineas_usuarios))
    print("amistades", len(lineas_amistad))
    print("solicitudes", len(lineas_sol), f"(usuarios tocados {len(con_sol)} / {n})")
    print("mensajes", len(lineas_msg), f"(usuarios tocados {len(con_msg)} / {n})")
    print("con fotos", f"{len(con_foto)} / {n}")
    print("demo:", demo, "/ clave123\n")
    return demo


def main():
    base = os.path.dirname(os.path.abspath(__file__))
    os.chdir(base)
    d1 = generar(100, "pequenio", 20261017)
    d2 = generar(1000, "mediano", 20261018)
    d3 = generar(5000, "grande", 20261019)
    print("listo")
    print("demo pequenio:", d1)
    print("demo mediano:", d2)
    print("demo grande:", d3)


if __name__ == "__main__":
    main()
