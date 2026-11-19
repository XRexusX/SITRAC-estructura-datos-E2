# pruebas.ps1 - Ejecuta las pruebas de SITRAC y verifica lo que el programa responde.
#
# Uso (desde la carpeta SITRAC, despues de compilar):
#     powershell -ExecutionPolicy Bypass -File tests\pruebas.ps1
#
# Cada prueba: se le "teclean" unas entradas al programa real (sitrac.exe), se
# captura lo que imprime y se comprueba que:
#   Esperado -> estos textos aparezcan,
#   Orden    -> estos textos aparezcan en ESE orden,
#   Ausente  -> estos textos NO aparezcan.
# El registro completo queda en tests\resultados.txt (evidencia de ejecucion).

param([string]$Exe = ".\sitrac.exe")

Set-Location (Split-Path -Parent $PSScriptRoot)   # carpeta SITRAC
if (-not (Test-Path $Exe)) {
    Write-Host "No existe $Exe. Compila primero (ver README)." -ForegroundColor Red
    exit 2
}

function Ejecutar([string[]]$lineas) {
    [IO.File]::WriteAllText("$PWD\tests\tmp_entrada.txt", (($lineas -join "`n") + "`n"))
    cmd /c "`"$Exe`" < tests\tmp_entrada.txt > tests\tmp_salida.txt"
    return [IO.File]::ReadAllText("$PWD\tests\tmp_salida.txt")
}

# Grafo de ejemplo usado por varias pruebas:
#   A-B 4 | A-C 1 | B-D 1 | C-D 5 | D-E 3      (F se agrega aparte, sin conexiones)
$grafo = @('1','1','A','1','B','1','C','1','D','1','E','0',
           '2','1','A','B','4','1','A','C','1','1','B','D','1','1','C','D','5','1','D','E','3','0')
$grafoConF = $grafo + @('1','1','F','0')

# Pasajeros del ABB: 50 Ana, 30 Beto, 70 Carla, 20 Diego, 40 Elena
$abb = @('6','1','50','Ana','20','1','30','Beto','20','1','70','Carla','20','1','20','Diego','20','1','40','Elena','20')

$pruebas = @(
 @{ Nombre='P01 (CP-01) Ciudad duplicada se rechaza sin crear nodo extra'
    Entradas=@(,@('1','1','Bogota','1','Bogota','3','0','0'))
    Esperado=@("[!] La ciudad 'Bogota' ya existe", 'Total: 1') },

 @{ Nombre='P02 (CP-02) Eliminar pasajero inexistente no rompe la lista'
    Entradas=@(,@('3','1','1','Ana','20','4','99','5','0','0'))
    Esperado=@('No existe un pasajero con ID 99. No se elimino nada', 'ID 1 | Ana | 20 anios', 'Total: 1') },

 @{ Nombre='P03 (CP-03) Atender / ver frente con la cola vacia'
    Entradas=@(,@('4','2','3','0','0'))
    Esperado=@('La cola esta vacia: no hay solicitudes por atender', '[!] La cola esta vacia.') },

 @{ Nombre='P04 (CP-04) Conexion con distancia negativa se rechaza'
    Entradas=@(,@('1','1','A','1','B','0','2','1','A','B','-5','4','0','0'))
    Esperado=@('Distancia invalida: debe ser un numero mayor que 0', '(no hay conexiones registradas)') },

 @{ Nombre='P05 (CP-05) Eliminar ciudad conectada borra sus conexiones'
    Entradas=@(,@('1','1','A','1','B','1','C','0','2','1','A','B','10','1','A','C','20','0','1','2','A','0','2','3','B','4','0','0'))
    Esperado=@('Ciudad eliminada junto con 2 conexion(es)', 'B no tiene conexiones', '(no hay conexiones registradas)') },

 @{ Nombre='P06 (CP-06) Letras donde va un numero: se recupera el menu'
    Entradas=@(,@('abc','3','1','hola','0','0'))
    Esperado=@('Opcion invalida. Elija un numero entre 0 y 9', 'se esperaba un numero entero', 'Hasta pronto.') },

 @{ Nombre='P07 ID de pasajero duplicado se rechaza'
    Entradas=@(,@('3','1','1','Ana','20','1','1','5','0','0'))
    Esperado=@('Ya existe un pasajero con ID 1. No se inserto', 'Total: 1') },

 @{ Nombre='P08 Encolar con ciudades sin ruta entre ellas se rechaza'
    Entradas=@(,@('1','1','A','1','B','0','3','1','1','Ana','20','0','4','1','1','A','B','4','0','0'))
    Esperado=@('No existe ruta entre A y B. Solicitud rechazada', '(la cola esta vacia)') },

 @{ Nombre='P09 ABB: recorridos inorden / preorden / postorden y altura'
    Entradas=@(,($abb + @('4','5','6','7','0','0')))
    Esperado=@('Inorden : 20 Diego, 30 Beto, 40 Elena, 50 Ana, 70 Carla',
               'Preorden : 50 Ana, 30 Beto, 20 Diego, 40 Elena, 70 Carla',
               'Postorden: 20 Diego, 40 Elena, 30 Beto, 70 Carla, 50 Ana',
               'Altura: 3 | Nodos: 5') },

 @{ Nombre='P10 ABB: eliminar nodo con dos hijos; lista y arbol quedan iguales'
    Entradas=@(,($abb + @('3','30','4','2','30','0','3','2','30','0','8','1','0','0')))
    Esperado=@('Pasajero 30 eliminado (lista y arbol)',
               'Inorden : 20 Diego, 40 Elena, 50 Ana, 70 Carla',
               'No existe un pasajero con ID 30 en el arbol',
               'No existe un pasajero con ID 30.',
               'Pasajeros (lista)    : 4',
               'Pasajeros (arbol)    : 4 | altura 3') },

 @{ Nombre='P11 BFS: orden de visita por niveles'
    Entradas=@(,($grafo + @('7','1','A','0','0')))
    Orden=@('nivel 0: A', 'nivel 1: B', 'nivel 1: C', 'nivel 2: D', 'nivel 3: E')
    Esperado=@('Ciudades alcanzadas: 5 de 5') },

 @{ Nombre='P12 DFS: orden de visita en profundidad (ciudad aislada no se alcanza)'
    Entradas=@(,($grafoConF + @('7','2','A','0','0')))
    Orden=@('    A', '      B', '        D', '          C', '          E')
    Esperado=@('Ciudades alcanzadas: 5 de 6') },

 @{ Nombre='P13 Dijkstra: camino y costo minimos; sin camino se informa'
    Entradas=@(,($grafoConF + @('7','3','A','E','3','A','F','0','0')))
    Esperado=@('Ruta mas corta: A -> B -> D -> E (8 km)', 'No existe ruta entre esas ciudades') },

 @{ Nombre='P14 Pila: deshacer devuelve el viaje al FRENTE de la cola'
    Entradas=@(,@('1','1','A','1','B','0','2','1','A','B','5','0',
                  '3','1','1','Ana','20','1','2','Luis','30','0',
                  '5','3','0',
                  '4','1','1','A','B','1','2','B','A','2','0',
                  '5','1','3','0',
                  '4','4','0','0'))
    Esperado=@('no hay nada que deshacer', 'Atendida: Ana (ID 1)', '1. Ana (ID 1): A -> B',
               'Deshecho: Ana (ID 1), A -> B volvio al FRENTE de la cola')
    Orden=@('Deshecho:', '1. Pasajero 1: A -> B', '2. Pasajero 2: B -> A') },

 @{ Nombre='P15 Guardar y luego cargar en otra ejecucion restaura todo (incluye el ABB)'
    Entradas=@(
      @('1','1','A','1','B','0','2','1','A','B','12.5','0','3','1','7','Zoe','25','0','4','1','7','A','B','0','9','1','tests/tmp_datos.txt','0','0'),
      @('9','2','tests/tmp_datos.txt','0','8','1','0','4','4','0','6','4','0','0'))
    Esperado=@("Datos guardados en 'tests/tmp_datos.txt'", 'Datos cargados desde',
               'Ciudades registradas : 2', 'Conexiones           : 1 (12.5 km en total)',
               'Pasajeros (lista)    : 1', 'Pasajeros (arbol)    : 1', 'Solicitudes en cola  : 1',
               '1. Pasajero 7: A -> B', 'Inorden : 7 Zoe') },

 @{ Nombre='P16 Archivo corrupto: no se carga y los datos actuales no cambian'
    Archivos=@{ 'tests/tmp_malo.txt' = "SITRAC|1`nC|A`nR|A|B|10`n" }
    Entradas=@(,@('3','1','1','Ana','20','0','9','2','tests/tmp_malo.txt','0','8','1','0','0'))
    Esperado=@('No se pudo cargar (los datos actuales no cambiaron): linea 3', 'Pasajeros (lista)    : 1') },

 @{ Nombre='P17 Archivo con basura o de otro tipo se rechaza'
    Archivos=@{ 'tests/tmp_basura.txt' = "hola mundo`n1|2|3`n" }
    Entradas=@(,@('9','2','tests/tmp_basura.txt','0','0'))
    Esperado=@('no es un archivo SITRAC valido') },

 @{ Nombre='P18 Archivo inexistente al cargar: se informa y el programa sigue'
    Entradas=@(,@('9','2','tests/no_existe_jamas.txt','0','0'))
    Esperado=@("no se pudo abrir 'tests/no_existe_jamas.txt'", 'Hasta pronto.') },

 @{ Nombre='P19 Opciones invalidas en menu principal y en submenu'
    Entradas=@(,@('99','1','7','0','0'))
    Esperado=@('Elija un numero entre 0 y 9', 'Elija un numero entre 0 y 3', 'Hasta pronto.') },

 @{ Nombre='P20 Distancia con letras se rechaza; entrada que se acaba no cuelga el programa'
    Entradas=@(@('1','1','A','1','B','0','2','1','A','B','abc','0','0'), @('1','1'))
    Esperado=@('se esperaba un numero (ej. 125.5)', '[Fin de la entrada]') }
)

$resultados = New-Object System.Text.StringBuilder
$fallidas = 0
foreach ($p in $pruebas) {
    if ($p.Archivos) {
        foreach ($k in $p.Archivos.Keys) { [IO.File]::WriteAllText("$PWD\$k", $p.Archivos[$k]) }
    }
    $salida = ''
    $n = 1
    foreach ($corrida in $p.Entradas) {
        $salida += "----- ejecucion $n (entradas: " + ($corrida -join ' , ') + ") -----`r`n"
        $salida += (Ejecutar $corrida)
        $n++
    }

    $faltan = @()
    foreach ($e in $p.Esperado) { if (-not $salida.Contains($e)) { $faltan += "falta: $e" } }
    if ($p.Orden) {
        $pos = 0
        foreach ($e in $p.Orden) {
            $i = $salida.IndexOf($e, $pos, [StringComparison]::Ordinal)
            if ($i -lt 0) { $faltan += "fuera de orden o ausente: $e"; break }
            $pos = $i + $e.Length
        }
    }
    foreach ($e in $p.Ausente) { if ($salida.Contains($e)) { $faltan += "no deberia aparecer: $e" } }

    $estado = if ($faltan.Count -eq 0) { 'PASA ' } else { 'FALLA' ; $fallidas++ }
    Write-Host "$estado $($p.Nombre)" -ForegroundColor $(if ($faltan.Count -eq 0) { 'Green' } else { 'Red' })
    foreach ($f in $faltan) { Write-Host "        $f" -ForegroundColor Red }

    [void]$resultados.AppendLine("=================================================================")
    [void]$resultados.AppendLine("$estado $($p.Nombre)")
    [void]$resultados.AppendLine("=================================================================")
    [void]$resultados.AppendLine($salida)
    foreach ($f in $faltan) { [void]$resultados.AppendLine("   >> $f") }
}

Remove-Item tests\tmp_* -ErrorAction SilentlyContinue
[IO.File]::WriteAllText("$PWD\tests\resultados.txt", $resultados.ToString())
$total = $pruebas.Count
Write-Host ""
Write-Host "Resultado: $($total - $fallidas) de $total pruebas pasan.  (registro en tests\resultados.txt)"
if ($fallidas -gt 0) { exit 1 }
