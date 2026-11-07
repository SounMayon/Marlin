# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris) — checkpoint dual-leveling

## Références figées

- Cible : MarlinFirmware/Marlin tag `2.1.2.9`, commit `7d300b42acadb3e7467e9f4b4fc03ebea592c2d0`.
- Source constructeur : SounMayon/Mizar-S, branche `main` (fork de Geeetech3D/Mizar-S ; le contenu doit encore être comparé avec le dépôt amont et la build rapportée sur l'imprimante).
- Branche de travail : `mizar-s-marlin-2.1.2.9`.
- Les branches `main` et `bugfix-2.1.x` ne sont pas modifiées.

## Commits présents (sauvegarde GitHub)

- `83a15f1` — début de portage Configuration.h : carte, port série, dimensions 255×255×260, fins de course, paramètres cinématiques, BLTouch, fonctions MBL/ABL, EEPROM/SD/TFT.
- `c42753d` — Z dual, reprise après coupure, pause avancée et options de capteur.
- `46520a3` — affectation PA15 au capteur filament natif (à contrôler avec la sélection de l'écran MKS).
- `b636a8a` — activation du second pilote Z.

Chaque modification de fichier a été enregistrée par un commit GitHub. Ce journal doit être actualisé après chaque grand bloc fonctionnel.

## Architecture de nivellement retrouvée dans le firmware Geeetech

Le constructeur ne se contente pas de définir simultanément les deux macros. Il a séparé les données MBL du singleton bilinéaire :
- l'objet ABL bilinéaire conserve le nom `bedlevel` ;
- l'objet MBL est nommé `mbl` dans `feature/bedlevel/mbl/mesh_bed_leveling.cpp` ;
- le choix runtime est piloté par `auto_manu_level_sel` ;
- `bedlevel.cpp` et `planner.cpp` appliquent le correctif actif selon ce sélecteur ;
- le traitement MBL G29 classique est désactivé et le G29 ABL de Geeetech route les appels entre ABL et MBL ;
- le MBL a des fonctions spécifiques de sauvegarde/restauration et d'interface LVGL.

C'est ce modèle qu'il faut porter, en adaptant les API au Marlin 2.1.2.9. Il ne faut pas enlever seulement le sanity check « select only one leveling option ».

## Build CI et configuration cible (checkpoint 2026-11-07)

- Le premier essai CI a révélé que `geeetech_lcd35` ne correspondait pas à un environnement accepté par le validateur de carte Marlin 2.1.2.9.
- L'environnement correct est `STM32F103VE_GTM32_maple`, dans `ini/stm32f1-maple.ini`, conformément à la définition GTM32.
- Le script de relocation Geeetech a été ajouté : vecteur d'interruption à `0x08007000` et linker adapté à la réservation bootloader.
- Le build GitHub Actions est désormais configuré pour compiler cet environnement. Les premiers échecs observés étaient des erreurs de sélection d'environnement ; ils ne constituent pas encore un diagnostic des erreurs C++.

### PLR constructeur

- Le Marlin 2.1.2.9 natif utilise un fichier `/PLR` sur la carte SD ; les options Geeetech `POWER_LOSS_STORAGE_EEPROM` et `POWER_LOSS_STORAGE_Z_VALUE` n'étaient pas implémentées dans cette base.
- Portage commencé : zone EEPROM distincte calculée après `SettingsData`, données de reprise protégées par CRC, chargement/sauvegarde/effacement adaptés, sauvegarde du Z physique avec correction de nivellement, et reprise avec restauration du Z absolu.
- La taille disponible est protégée par un `static_assert` à la compilation. Ce point doit être validé par le build CI et le comportement réel de la carte.

## Sauvegardes / jalons du portage (checkpoint 2026-11-07)

Les changements suivants sont déjà committés sur la branche dédiée :

- Base Mizar S : carte, géométrie, axes, BLTouch, écran MKS LVGL, stockage SD/EEPROM et premiers réglages de pause/PLR.
- Création de `MIZAR_S_DUAL_LEVELING` et d'un sélecteur runtime `auto_manu_level_sel`.
- Deux objets de maillage indépendants : `bedlevel` (ABL bilinéaire) et `mbl` (maillage manuel).
- `planner.cpp` applique la correction de la grille sélectionnée.
- `G29` et `M421` dispatchent vers des handlers spécifiques ABL ou MBL ; `G42` suit les coordonnées du maillage actif.
- `M420` gère sélection, affichage et recentrage de la grille active.
- `M500/M501` enregistrent/restaurent séparément la grille manuelle et le sélecteur de mode. `M502` réinitialise le mode vers ABL.
- La version EEPROM de la configuration a été incrémentée de `V88` à `V89`, car le schéma de stockage a changé.

**Conséquence au premier flash :** ne pas réutiliser aveuglément les anciens paramètres EEPROM. Charger les valeurs par défaut (`M502`), puis sauvegarder (`M500`) après vérification du démarrage et du comportement des axes. Les anciens décalages/profils peuvent devoir être restaurés manuellement à partir d'une sauvegarde préalable.

Ces jalons sont sauvegardés dans l'historique Git de la branche. L'état de compilation reste non vérifié.

## État réel — non validé matériellement

- La branche comporte un premier jeu de paramètres Mizar, mais ceux-ci sont encore préliminaires.
- Le noyau dual MBL/ABL (objets, planificateur, G29/M421/G42/M420 et persistance de base) est maintenant intégré statiquement, mais reste à compiler et à tester. L'interface LVGL guidée et les séquences de retour/annulation ne sont pas terminées.
- Le parcours EEPROM utilise les zones natives MBL et ABL distinctes, plus un booléen de sélection ; le schéma est marqué V89 et nécessite M502/M500 au premier démarrage.
- Le mapping runout doit être contrôlé avec les deux branches de pins (LVGL et non-LVGL) : l'écran tactile utilise PA4, donc PA4 ne doit jamais être affecté au capteur filament.
- Le circuit de détection de perte secteur utilise PE3 dans la configuration LVGL de la carte. L'état électrique réel et les pull-up/pull-down doivent être validés sur carte.
- La ressource TFT/LVGL et les langues françaises/anglaises ne sont pas encore réintégrées/validées.
- Les options de pause/runout et la reprise PLR ont été activées en première passe seulement ; leurs séquences doivent être auditées avant d'être déclarées correctes.
- Aucune compilation PlatformIO ni aucun essai imprimante n'a été exécuté depuis cette session. Les actions de compilation/flash et les essais physiques resteront à effectuer sur PC/imprimante.

## Prochaine séquence

1. Terminer et adapter les écrans MKS LVGL de sélection auto/manuelle, d'ABL 4x4 et de MBL 4x4, y compris annulation/retour et valeurs sauvegardées.
2. Vérifier toutes les références `auto_manu_level_sel` et les callbacks UI, puis tester l'interaction G29/M420/M421/G42 après compilation.
3. Auditer le chemin d'exécution runout/pause et celui du PLR (PE3, SDIO, fichier recovery, arrêt et redémarrage).
4. Migrer les fichiers UI/assets en respectant l'organisation 2.1.2.9 et ne garder que français/anglais.
5. Vérifier le linker, l'environnement PlatformIO et la configuration de build STM32F103VET6.
6. Ajouter une procédure de compilation, de flash, de sauvegarde EEPROM et un protocole de validation sur banc.

## Règle de livraison

Ne jamais marquer une fonction comme « terminée » tant que la cohérence du code, sa compilation dans l'environnement visé et, lorsque nécessaire, un essai réel n'ont pas été réalisés.