# genera pequenio (100), mediano (1000) y grande (5000)
# fechas, claves y relaciones variadas; sin duplicados

import os
import random
from datetime import date, datetime, timedelta

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

# rango amplio para que el video pueda buscar >= 6 semanas
FECHA_MIN = date(2026, 6, 1)
FECHA_MAX = date(2026, 10, 15)


def dias_mes(anio, mes):
    if mes == 2:
        if anio % 4 == 0 and (anio % 100 != 0 or anio % 400 == 0):
            return 29
        return 28
    if mes in (4, 6, 9, 11):
        return 30
    return 31


def fecha14_aleatoria(usadas, inicio=None, fin=None):
    """AAAAMMDDhhmmss unica; dia al azar dentro del rango."""
    a = inicio or FECHA_MIN
    b = fin or FECHA_MAX
    span = (b - a).days
    for _ in range(200):
        d = a + timedelta(days=random.randint(0, max(span, 0)))
        hh = random.randint(7, 22)
        mm = random.randint(0, 59)
        ss = random.randint(0, 59)
        txt = f"{d.year:04d}{d.month:02d}{d.day:02d}{hh:02d}{mm:02d}{ss:02d}"
        if txt not in usadas:
            usadas.add(txt)
            return txt
    # si choco mucho, avanza segundo a segundo
    base = datetime(a.year, a.month, a.day, 8, 0, 0)
    while True:
        txt = base.strftime("%Y%m%d%H%M%S")
        if txt not in usadas:
            usadas.add(txt)
            return txt
        base += timedelta(seconds=1)


def clave_unica(usadas, i):
    # todas distintas; la del demo (i==0 despues de ordenar nombres) se fija aparte
    prefijos = ["clave", "pass", "uda", "info", "poo", "red", "book", "foto"]
    while True:
        c = f"{random.choice(prefijos)}{random.randint(1000, 9999)}{i % 97}"
        if c not in usadas and len(c) <= 32:
            usadas.add(c)
            return c


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

    claves_usadas = set()
    # demo con clave fija para el video
    clave_demo = "clave123"
    claves_usadas.add(clave_demo)

    lineas_usuarios = []
    for i in range(n):
        anio_n = random.randint(1988, 2005)
        mes_n = random.randint(1, 12)
        dia_n = random.randint(1, dias_mes(anio_n, mes_n))
        nacimiento = f"{anio_n:04d}{mes_n:02d}{dia_n:02d}{random.randint(0,23):02d}{random.randint(0,59):02d}{random.randint(0,59):02d}"

        anio_i = random.choice([2024, 2025, 2026])
        mes_i = random.randint(1, 12)
        dia_i = random.randint(1, dias_mes(anio_i, mes_i))
        inscripcion = f"{anio_i:04d}{mes_i:02d}{dia_i:02d}{random.randint(8,20):02d}{random.randint(0,59):02d}{random.randint(0,59):02d}"

        clave = clave_demo if i == 0 else clave_unica(claves_usadas, i)
        correo = f"{usados[i]}@udea.edu.co"
        ciudad = random.choice(CIUDADES)
        lineas_usuarios.append(
            f"{usados[i]}|{nacimiento}|{correo}|{clave}|{ciudad}|Colombia|{inscripcion}"
        )

    amigos = [[False] * n for _ in range(n)]

    def marcar(a, b):
        if a == b:
            return
        if a > b:
            a, b = b, a
        amigos[a][b] = True
        amigos[b][a] = True

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

    for u in range(n):
        cuantos = random.randint(2, 5)
        for _ in range(cuantos):
            marcar(u, random.randint(0, n - 1))

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

    # solicitudes al demo (mezcladas, no siempre los mismos)
    candidatos = list(range(1, n))
    random.shuffle(candidatos)
    for emisor in candidatos[: min(18, len(candidatos))]:
        agregar_sol(emisor, 0)
    random.shuffle(candidatos)
    for receptor in candidatos[: min(12, len(candidatos))]:
        agregar_sol(0, receptor)

    # casi todos participan en alguna solicitud, pares al azar
    orden = list(range(n))
    random.shuffle(orden)
    for u in orden[: int(n * 0.88)]:
        for _ in range(random.randint(1, 3)):
            otro = random.randint(0, n - 1)
            if random.random() < 0.5:
                agregar_sol(u, otro)
            else:
                agregar_sol(otro, u)

    random.shuffle(lineas_sol)

    claves_msg = set()
    lineas_msg = []
    fechas_usadas = set()

    def agregar_msg(emisor, receptor, texto, fecha14=None):
        if emisor == receptor or not texto:
            return False
        if fecha14 is None:
            fecha14 = fecha14_aleatoria(fechas_usadas)
        elif fecha14 in fechas_usadas:
            fecha14 = fecha14_aleatoria(fechas_usadas)
        else:
            fechas_usadas.add(fecha14)
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

    # 30 mensajes al demo cada 2 dias desde ago-1 (>= 6 semanas)
    amigos_demo = lista_amigos(0) or list(range(1, min(11, n)))
    inicio_demo = date(2026, 8, 1)
    for k in range(30):
        d = inicio_demo + timedelta(days=k * 2)
        hh = 10 + (k % 8)
        mm = (k * 7) % 60
        ss = (k * 13) % 60
        fecha = f"{d.year:04d}{d.month:02d}{d.day:02d}{hh:02d}{mm:02d}{ss:02d}"
        texto = FRASES[k % len(FRASES)]
        if k % 4 == 0:
            texto = f"{texto} ({usados[amigos_demo[k % len(amigos_demo)]]})"
        agregar_msg(amigos_demo[k % len(amigos_demo)], 0, texto, fecha)

    # resto: casi todos envian/reciben en fechas sueltas
    for u in range(n):
        amigos_u = lista_amigos(u)
        if not amigos_u:
            amigos_u = [(u + 1) % n]
        for t in range(1 + (u % 2)):
            dest = amigos_u[(u + t) % len(amigos_u)]
            frase = FRASES[random.randint(0, len(FRASES) - 1)]
            agregar_msg(u, dest, f"{frase} [{usados[u]} #{u}-{t}]")
        origen = amigos_u[random.randint(0, len(amigos_u) - 1)]
        frase2 = FRASES[random.randint(0, len(FRASES) - 1)]
        agregar_msg(origen, u, f"{frase2} para {usados[u]} #{random.randint(1, 9999)}")

    lineas_msg.sort(key=lambda L: L.split("|")[2])

    lineas_media = []
    lineas_cambios = []
    id_foto = 0
    fechas_foto = set()

    def agregar_album(usuario, nombre, privacidad, cantidad, likes_base):
        nonlocal id_foto
        lineas_media.append(f"A|{usuario}|{nombre}|{privacidad}")
        for f in range(1, cantidad + 1):
            fecha = fecha14_aleatoria(fechas_foto, date(2026, 5, 1), date(2026, 10, 10))
            archivo = f"{nombre}_{f}"
            bytes_f = 900 + random.randint(50, 4000) + f * 17 + (usuario % 50)
            lineas_media.append(f"F|{fecha}|{bytes_f}|{archivo}")
            likes = likes_base + f + random.randint(0, 5)
            if likes > 0:
                lineas_cambios.append(f"K|{id_foto}|{likes}")
            id_foto += 1

    if n > 1:
        agregar_album(1, "viaje_cartagena", "A", 5, 2)
        agregar_album(1, "campus", "P", 5, 1)
        agregar_album(1, "privado", "R", 2, 0)

    tope_fotos = int(n * 0.90)
    for u in range(n):
        if u == 1 or u >= tope_fotos:
            continue
        cuantos_alb = random.randint(1, 2)
        for a in range(cuantos_alb):
            nom = f"{random.choice(ALBUMES)}_{u}_{a}"
            priv = random.choice(["A", "A", "P"])
            fotos = random.randint(1, 3)
            agregar_album(u, nom, priv, fotos, random.randint(0, 4))

    lineas_cambios = [f"L|0|10"] + lineas_cambios

    def escribir(nombre, lineas):
        with open(os.path.join(carpeta, nombre), "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(lineas))
            if lineas:
                f.write("\n")

    assert len(lineas_usuarios) == len(set(lineas_usuarios))
    assert len(lineas_amistad) == len(set(lineas_amistad))
    assert len(lineas_sol) == len(set(lineas_sol))
    assert len(lineas_msg) == len(set(lineas_msg))
    assert len(claves_usadas) == n

    escribir("usuarios.txt", lineas_usuarios)
    escribir("multimedia.txt", lineas_media)
    escribir("amistades.txt", lineas_amistad)
    escribir("solicitudes.txt", lineas_sol)
    escribir("mensajes.txt", lineas_msg)
    escribir("cambios.txt", lineas_cambios)

    msgs_demo = [L for L in lineas_msg if L.split("|")[1] == "0"]
    dias_demo = sorted({L.split("|")[2][:8] for L in msgs_demo})

    print(f"=== {carpeta} ===")
    print("usuarios", len(lineas_usuarios), "| claves distintas", len(claves_usadas))
    print("amistades", len(lineas_amistad))
    print("solicitudes", len(lineas_sol))
    print("mensajes", len(lineas_msg))
    print("dias distintos msgs demo:", len(dias_demo), "| desde", dias_demo[0], "hasta", dias_demo[-1])
    print("demo:", demo, "/", clave_demo, "\n")
    return demo, clave_demo


def main():
    base = os.path.dirname(os.path.abspath(__file__))
    os.chdir(base)
    d1 = generar(100, "pequenio", 20261020)
    d2 = generar(1000, "mediano", 20261021)
    d3 = generar(5000, "grande", 20261022)
    print("listo")
    print("demo pequenio:", d1[0], d1[1])
    print("demo mediano:", d2[0], d2[1])
    print("demo grande:", d3[0], d3[1])


if __name__ == "__main__":
    main()
