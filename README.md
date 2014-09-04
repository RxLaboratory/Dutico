Dutico
======

Duduf Timecode Manager

Dutico est une application développée par Duduf - www.duduf.com - qui permet de créer un pont entre des logiciels de montage et de compositing.

• Création de composition After Effects depuis un montage provenant de tout logiciel de montage (via un EDL), avec importation automatique des médias, acceptant plusieurs calques par plan.

• Incrustations automatique de textes (titres, sous-titres, infos) dans des vidéos (via After Effects).

• Conversions de fichiers de timecodes.

• Création de textes quicktimes.

Dutico est capable de reconnaitre/modifier/créer toutes sortes de fichiers de timecodes et sous-titres (EDL, Avid Caption Pugin, Quicktime Text...) et de les convertir entre eux.
Dutico permet aussi de créer une composition After Effects à partir de ces timecodes, avec une détection automatique des médias, tout en gérant de nombreuses options de création et avec la possibilité d'utiliser plusieurs calques par plan.

____
Timecodes gérés par Dutico :

• Edit Decision List (EDL) : c'est un grand classique des formats d'échanges audiovisuels, que tous les logiciels de montage peuvent créer, ce qui permet, en utilisant ce type de timecode, de créer un pont entre n'importe quels logiciels de montage ou de compositing.

• Avid Caption Plugin : ce type de timecode est généré par les logiciels de montage Avid, il permet, grâce à un effet sur une piste vidéo dans les logiciels de montage Avid de créer précisément les timecodes que l'on veut (pour du sous-titrage, ou pour toute autre raison qui empèche d'utiliser un EDL ou qui nécessite de changer les timecodes des plans)

• Quicktime Text : ce type de timecode permet d'insérer du texte sur une vidéo Apple Quicktime (pour des sous-titres, des timecodes, des informations...)


Developpers:
====

"jsx" folder contains Adobe Extendscript Files needed to import DTC files in After Effects and convert them to compositions. Those files must be placed inside the installation folder of Dutico, alongside the exe.

"resources" folder contains a blank DTC project (.dtcp). It's an SQLite DB.
