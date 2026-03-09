 Compilation:
```
cmake -B build -D CMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
```
`Release`  can be changed to `Debug` if necessary.



2. tabella virtuale in testa :(
rettangolo text view parte di stato del programma,  sf::IntRect text_view e Vector2i cursor_pos

funzione di disegno:
doGrapgics (State gs)
{
    gs.window.clear();
    ...
    gs.window.display();
}


in costruttore di stato aggiunti text_view a {0,0}, w/FONT_WIDTH -2 e stesso per altezza come size. (w e h sono window.getSize.x e y, ma costruttore lo sa già) con static_cast<int> perché cose in unsigned int non le vuole
cursor_pos va a 0,0

ora al posto di cancellare testo di tiene tutto, togliere linee che cancellano log e modificare le altre per non disegnare tutto buffer ma solo porzione compresa in finestra
Usando paramentri di text_view si pescano caratteri da stampare

8. Si aggiunge cursore, lui lo disegna con underscore _ verde
Più avanti si potrà spostare.
cursor_pos ci dirà come si trova in tabella, poi va convertito in coordinate finestra, FONT_WIDTH e altro sono scala per passare da uno ad altro.

Arrivare fino a punto 9
9. in handle di Text Entered dobbiamo gestire accapo spostando cursore in nuova riga, o per altri caratteri spostarlo avanti di uno.

In do Graphics
lines_to_print=min(cast unsigned (gs.log.size()-gs.text_view_position.y), cast unsigned (gs.text_view, size.y));
for for(lines to print)
{
    logText.setPosition(FONTWIDTH, cast float(i*FONTSIZE)+FONTIZE);
    if(test_view.position.x<log[i+gs.textview.position.y]size())    //se c'è qualcosa da stampare
    {
        logText.setString(gs.log[i+t_v.y].substr((size_t)gs.v_t.position.x, (size_t)gs.v_t.size.x));
        gs.window.draw(logText);
    }
}

logText.setString("_");

setFillColor
setPosition(castfloat(gs.cursor_pos.x-gs.text_view.position.x+1)*FW, castfloat...)

Quando uno batte carattere gs.log[gs.cursor_pos.y].insert(gs.cursor_pos.x blabla)