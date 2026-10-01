# Qué quiero

Un sistema que me permita meter la mayor cantidad de tenants posible dentro de una sola VM aislada, sin que se pisen entre ellos y sin desperdiciar recursos.

Quiero que ese sistema conviva con Linux, no que lo reemplace. Linux planifica lo grueso; Pulsar decide lo fino dentro de su espacio.

# Cómo quiero que se comporte

Quiero que las operaciones se atiendan de forma justa y ordenada, pero con inteligencia:

* Las operaciones cortas o que esperan I/O deben pasar primero. Son rápidas, liberan recursos, y mantienen el sistema ágil.

* Las operaciones largas (reportes, backups, procesos pesados) van al final. No porque sean menos importantes, sino porque no bloquean a nadie mientras corren.

* Ninguna operación debería morir de hambre. Si algo lleva mucho esperando, tiene que subir.

Quiero una estructura de prioridades con varios niveles (pienso en tres: alto, medio, bajo) donde las operaciones puedan moverse entre niveles según cómo se comporten. Si una operación se porta bien y es rápida, sube. Si consume y se alarga, baja.

Quiero además una vía de retorno para las operaciones que salieron a esperar I/O: cuando vuelven, no deberían empezar de cero. Deberían reincorporarse con ventaja.

# Cómo quiero que se ejecute

No quiero que el sistema cree procesos a lo loco cuando llega trabajo. Quiero que ya tenga trabajadores listos esperando, y que esos trabajadores solo reciban la información necesaria para hacer su parte.

La idea es que crear trabajo no sea caro, y que aceptar trabajo tampoco lo sea. La información necesaria para ejecutar algo debería viajar con ese algo desde el momento en que entra al sistema.

# Qué me importa

* Densidad: más tenants por VM, sin sacrificar estabilidad.

* Fluidez: lo pequeño y lo urgente no espera detrás de lo grande.

* Equidad: nadie se queda atrás para siempre.

*  Convivencia con Linux: no pelear con el sistema operativo, complementarlo.

# Qué no me importa (por ahora)

* Cómo se implementa cada pieza.

* Qué componentes exactos tendrá.

* Si uso hilos, procesos, coroutines o cualquier otra cosa.

*  Detalles de rendimiento medibles. Eso vendrá cuando haya algo que medir.

# Lo que aún no tengo claro

* Cómo exactamente voy a coordinar a Pulsar con Linux sin que se estorben.

* Cómo voy a saber, al momento de aceptar una operación, si es "corta" o "larga".

* Cuántos niveles de prioridad son realmente necesarios (¿tres? ¿cinco? ¿depende?).

* Qué información exacta debería llevar cada operación para que el sistema decida bien.

* Cómo limitar y compartir los recursos.