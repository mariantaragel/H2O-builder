# H2O builder
## Popis projektu
Molekuly vody jsou vytvářeny ze dvou atomů vodíku a jednoho atomu kyslíku. V systému jsou tři typy procesů: (0) hlavní proces, (1) kyslík a (2) vodík. Po vytvoření procesů se procesy reprezentující kyslíky a vodíky řadí do dvou front—jedna pro kyslíky a druhá pro vodíky. Ze začátku fronty vždy vystoupí jeden kyslík a dva vodíky a vytvoří molekulu. V jednu chvíli je možné vytvářet pouze jednu molekulu. Po jejím vytvoření je prostor uvolněn dalším atomům pro vytvoření další molekuly. Procesy, které vytvořily molekulu následně končí. Ve chvíli, kdy již není k dispozici dostatek atomů kyslíku nebo vodíku pro další molekulu (a ani žádné další již nebudou hlavním procesem vytvořeny) jsou všechny zbývající atomy kyslíku a vodíku uvolněny z front a procesy jsou ukončeny.

## Detailní specifikace
### Spuštění
```sh
$ ./proj2 NO NH TI TB
```
<ul>
  <li>NO: Počet kyslíků</li>
  <li>NH: Počet vodíků</li>
  <li>TI: Maximální čas milisekundách, po který atom kyslíku/vodíku po svém vytvoření čeká, než se zařadí do fronty na vytváření molekul. 0<=TI<=1000</li>
  <li>TB: Maximální čas v milisekundách nutný pro vytvoření jedné molekuly. 0<=TB<=1000</li>
</ul>

### Chybové stavy
Pokud některý ze vstupů nebude odpovídat očekávanému formátu nebo bude mimo povolený rozsah, program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované zdroje a ukončí se s kódem (exit code) 1. Pokud selže některá z operací se semafory, nebo sdílenou pamětí, tak program postupujte stejně.

### Implementační detaily
<ul>
  <li>Každý proces vykonává své akce a současně zapisuje informace o akcích do souboru s názvem proj2.out. Součástí výstupních informací o akci je pořadové číslo A prováděné akce (viz popis výstupů). Akce se číslují od jedničky.</li>
  <li>Použijte sdílenou paměť pro implementaci čítače akcí a sdílených proměnných nutných pro synchronizaci.</li>
  <li>Použijte semafory pro synchronizaci procesů.</li>
  <li>Nepoužívejte aktivní čekání (včetně cyklického časového uspání procesu) pro účely synchronizace.</li>
  <li>Pracujte s procesy, ne s vlákny.</li>
</ul>

### Hlavní proces
<ul>
  <li>Hlavní proces vytváří ihned po spuštění NO procesů kyslíku a NH procesů vodíku.</li>
  <li>Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy ukončeny, ukončí se i hlavní proces s kódem (exit code) 0.</li>
</ul>

### Proces Kyslík
<ul>
  <li>Každý kyslík je jednoznačně identifikován číslem idO, 0<idO<=NO</li>
  <li>Po spuštění vypíše: <em>A: O idO: started</em></li>
  <li>Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TI></li>
  <li>Vypíše: <em>A: O idO: going to queue</em> a zařadí se do fronty kyslíků na vytváření molekul.</li>
  <li>Ve chvíli, kdy není vytvářena žádná molekula, jsou z čela front uvolněny kyslík a dva vodíky. Příslušný proces po uvolnění vypíše: <em>A: O idO: creating molecule noM</em> (noM je číslo molekuly, ty jsou číslovány postupně od 1).</li>
  <li>Pomocí usleep na náhodný čas v intervalu <0,TB> simuluje dobu vytváření molekuly.</li>
  <li>Po uplynutí času vytváření molekuly informuje vodíky ze stejné molekuly, že je molekula dokončena.</li>
  <li>Vypíše: <em>A: O idO: molecule noM created</em> a proces končí.</li>
  <li>Pokud již není k dispozici dostatek vodíků (ani nebudou žádné další vytvořeny/zařazeny do fronty) vypisuje: <em>A: O idO: not enough H</em> a proces končí.</li>
</ul>

### Proces Vodík
<ul>
  <li>Každý vodík je jednoznačně identifikován číslem idH, 0<idH<=NH</li>
  <li>Po spuštění vypíše: <em>A: H idH: started</em></li>
  <li>Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TI></li>
  <li>Vypíše: <em>A: H idH: going to queue</em> a zařadí se do fronty vodíků na vytváření molekul.</li>
  <li>Ve chvíli, kdy není vytvářena žádná molekula, jsou z čela front uvolněny kyslík a dva vodíky. Příslušný proces po uvolnění vypíše: <em>A: H idH: creating molecule noM</em> (noM je číslo molekuly, ty jsou číslovány postupně od 1).</li>
  <li>Následně čeká na zprávu od kyslíku, že je tvorba molekuly dokončena.</li>
  <li>Vypíše: <em>A: H idH: molecule noM created</em> a proces končí.</li>
  <li>Pokud již není k dispozici dostatek kyslíků nebo vodíků (ani nebudou žádné další vytvořeny/zařazeny do fronty) vypisuje: <em>A: H idH: not enough O or H</em> a process končí.</li>
</ul>

## Příklady použití
Příklad výstupního souboru proj2.out pro následující příkaz:
```sh
$ ./proj2 3 5 100 100
```
<hr><em>
1: H 1: started<br>
2: H 3: started<br>
3: O 1: started<br>
4: O 1: going to queue<br>
5: H 2: started<br>
6: H 2: going to queue<br>
7: H 1: going to queue<br>
8: O 3: started<br>
9: O 3: going to queue<br>
10: H 5: started<br>
11: H 4: started<br>
12: O 2: started<br>
13: H 1: creating molecule 1<br>
14: H 4: going to queue<br>
15: O 1: creating molecule 1<br>
16: H 2: creating molecule 1<br>
17: H 5: going to queue<br>
18: H 2: molecule 1 created<br>
19: H 1: molecule 1 created<br>
20: O 1: molecule 1 created<br>
21: H 3: going to queue<br>
22: O 3: creating molecule 2<br>
23: O 2: going to queue<br>
24: H 4: creating molecule 2<br>
25: H 5: creating molecule 2<br>
26: O 3: molecule 2 created<br>
27: H 4: molecule 2 created<br>
28: H 5: molecule 2 created<br>
29: H 3: not enough O or H<br>
30: O 2: not enough H</em>
