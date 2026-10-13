#genera los 6 txt de prueba. hay que correrlo con el directorio de trabajo en esta carpeta datos
#en rstudio: session > set working directory > to source file location, y luego source()

set.seed(20261012)

n <- 1000
salida <- "conjunto"
if (!dir.exists(salida)) {
  dir.create(salida)
}

nombreUsuario <- function(i) {
  sprintf("u%04d", i)
}

#usuarios ya salen ordenados porque el nombre lleva ceros
ciudades <- c("Medellin", "Bogota", "Cali", "Barranquilla")
lineasUsuarios <- character(n)
for (i in 0:(n - 1)) {
  nacimiento <- sprintf("%04d%02d%02d000000", 1985 + (i %% 20), (i %% 12) + 1, (i %% 27) + 1)
  inscripcion <- sprintf("2024%02d%02d000000", (i %% 12) + 1, (i %% 27) + 1)
  lineasUsuarios[i + 1] <- paste(
    nombreUsuario(i), nacimiento,
    paste0("u", i, "@mail.com"),
    "clave123",
    ciudades[(i %% 4) + 1],
    "Colombia",
    inscripcion,
    sep = "|"
  )
}

#amistad guarda el par con el indice menor primero, una sola vez
amigos <- matrix(FALSE, nrow = n, ncol = n)
marcarAmistad <- function(a, b) {
  if (a == b) {
    return()
  }
  if (a > b) {
    tmp <- a
    a <- b
    b <- tmp
  }
  amigos[a + 1, b + 1] <<- TRUE
  amigos[b + 1, a + 1] <<- TRUE
}

#u0000 queda con tres grupos de amigos para que el sorteo tenga los tres pesos
#0..24 es un grupo cerrado: entre ellos hay mas de 21 amigos en comun
for (a in 0:23) {
  for (b in (a + 1):24) {
    marcarAmistad(a, b)
  }
}
#25..34 son amigos de u0000 y de 15 del grupo, caen en el peso 2.5
for (a in 25:34) {
  marcarAmistad(0, a)
  for (b in 1:15) {
    marcarAmistad(a, b)
  }
}
#35..44 son amigos de u0000 y de 5 del grupo, caen en el peso 1
for (a in 35:44) {
  marcarAmistad(0, a)
  for (b in 1:5) {
    marcarAmistad(a, b)
  }
}
#el resto tiene algunas amistades sueltas, sin tocar el grado de u0000
for (k in 1:800) {
  a <- sample(45:(n - 1), 1)
  b <- sample(45:(n - 1), 1)
  marcarAmistad(a, b)
}

#la matriz es simetrica, asi que cada amistad se escribe una sola vez, con el indice menor primero
pares <- which(amigos, arr.ind = TRUE)
pares <- pares[pares[, 1] < pares[, 2], , drop = FALSE]
lineasAmistad <- paste(pares[, 1] - 1, pares[, 2] - 1, sep = "|")

sonAmigos <- function(a, b) {
  amigos[a + 1, b + 1]
}

#solicitudes: ni a un amigo, ni duplicadas, ni la ida y la vuelta a la vez
clavesSolicitud <- character(0)
lineasSolicitud <- character(0)
agregarSolicitud <- function(emisor, receptor) {
  if (emisor == receptor || sonAmigos(emisor, receptor)) {
    return()
  }
  ida <- paste(emisor, receptor, sep = "-")
  vuelta <- paste(receptor, emisor, sep = "-")
  if (ida %in% clavesSolicitud || vuelta %in% clavesSolicitud) {
    return()
  }
  clavesSolicitud <<- c(clavesSolicitud, ida)
  lineasSolicitud <<- c(lineasSolicitud, paste(emisor, receptor, sep = "|"))
}

for (emisor in 200:214) {
  agregarSolicitud(emisor, 0)
}
for (receptor in 220:229) {
  agregarSolicitud(0, receptor)
}
for (k in 1:40) {
  agregarSolicitud(sample(50:(n - 1), 1), sample(50:(n - 1), 1))
}

#mensajes en orden de fecha. cada uno tiene segundo distinto, asi no se repite la linea
lineasMensaje <- character(0)
claveMensaje <- character(0)
agregarMensaje <- function(emisor, receptor, fecha14, texto) {
  if (emisor == receptor || nchar(texto) == 0 || nchar(texto, type = "bytes") > 512) {
    return()
  }
  clave <- paste(emisor, receptor, fecha14, texto, sep = "|")
  if (clave %in% claveMensaje) {
    return()
  }
  claveMensaje <<- c(claveMensaje, clave)
  largo <- nchar(texto, type = "bytes")
  lineasMensaje <<- c(lineasMensaje, paste(emisor, receptor, fecha14, largo, texto, sep = "|"))
}

#30 mensajes para u0000 repartidos en mas de 6 semanas
for (k in 0:29) {
  dia <- 1 + k * 2
  mes <- 8
  if (dia > 31) {
    dia <- dia - 31
    mes <- 9
  }
  fecha <- sprintf("2026%02d%02d120000", mes, dia)
  agregarMensaje(1 + (k %% 10), 0, fecha, paste0("mensaje", k))
}
for (k in 0:19) {
  fecha <- sprintf("202607%02d153000", k + 1)
  agregarMensaje(50 + k, 60 + k, fecha, paste0("nota", k))
}

#bruno del mini es u0001: dos albumes de 5 y uno privado, para la demo del perfil
lineasMedia <- character(0)
lineasCambios <- character(0)
idFoto <- 0
agregarAlbum <- function(usuario, nombre, privacidad, cantidad, likesBase) {
  lineasMedia <<- c(lineasMedia, paste("A", usuario, nombre, privacidad, sep = "|"))
  for (f in 1:cantidad) {
    fecha <- sprintf("202608%02d101500", f)
    archivo <- paste0(nombre, f)
    lineasMedia <<- c(lineasMedia, paste("F", fecha, 1000 + f, archivo, sep = "|"))
    if (likesBase + f > 0) {
      lineasCambios <<- c(lineasCambios, paste("K", idFoto, likesBase + f, sep = "|"))
    }
    idFoto <<- idFoto + 1
  }
}

agregarAlbum(1, "viaje", "A", 5, 1)
agregarAlbum(1, "publico", "P", 5, 0)
agregarAlbum(1, "secreto", "R", 2, 0)
for (u in seq(10, 40, by = 2)) {
  agregarAlbum(u, "general", "A", 1, 0)
}

#u0000 ya leyo los primeros 10, le quedan 20 sin leer
lineasCambios <- c(paste("L", 0, 10, sep = "|"), lineasCambios)

escribir <- function(nombre, lineas) {
  ruta <- file.path(salida, nombre)
  #useBytes evita que R cambie los acentos y el salto de linea
  writeLines(lineas, ruta, useBytes = TRUE)
}

if (anyDuplicated(lineasUsuarios) > 0 || anyDuplicated(lineasAmistad) > 0 ||
    anyDuplicated(lineasSolicitud) > 0 || anyDuplicated(lineasMensaje) > 0) {
  stop("se colaron duplicados, no escribo los archivos")
}

escribir("usuarios.txt", lineasUsuarios)
escribir("multimedia.txt", lineasMedia)
escribir("amistades.txt", lineasAmistad)
escribir("solicitudes.txt", lineasSolicitud)
escribir("mensajes.txt", lineasMensaje)
escribir("cambios.txt", lineasCambios)

cat("listo\n")
cat("usuarios", length(lineasUsuarios), "\n")
cat("amistades", length(lineasAmistad), "\n")
cat("solicitudes", length(lineasSolicitud), "\n")
cat("mensajes", length(lineasMensaje), "\n")
cat("la clave de todos es clave123 y el de la demo es u0000\n")
