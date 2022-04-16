# H2O builder
## Popis projektu
Molekuly vody jsou vytvářeny ze dvou atomů vodíku a jednoho atomu kyslíku. V systému jsou tři typy procesů: (0) hlavní proces, (1) kyslík a (2) vodík. Po vytvoření procesů se procesy reprezentující kyslíky a vodíky řadí do dvou front—jedna pro kyslíky a druhá pro vodíky. Ze začátku fronty vždy vystoupí jeden kyslík a dva vodíky a vytvoří molekulu. V jednu chvíli je možné vytvářet pouze jednu molekulu. Po jejím vytvoření je prostor uvolněn dalším atomům pro vytvoření další molekuly. Procesy, které vytvořily molekulu následně končí. Ve chvíli, kdy již není k dispozici dostatek atomů kyslíku nebo vodíku pro další molekulu (a ani žádné další již nebudou hlavním procesem vytvořeny) jsou všechny zbývající atomy kyslíku a vodíku uvolněny z front a procesy jsou ukončeny.

## Detailní specifikace
### Spuštění
```
$ ./proj2 NO NH TI TB
```
<ul>
  <li>NO: Počet kyslíků</li>
  <li>NH: Počet vodíků</li>
  <li>TI: Maximální čas milisekundách, po který atom kyslíku/vodíku po svém vytvoření čeká, než se zařadí do fronty na vytváření molekul. 0<=TI<=1000</li>
  <li>TB: Maximální čas v milisekundách nutný pro vytvoření jedné molekuly. 0<=TB<=1000</li>
</ul>

## Příklady použití
