# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris) — audit statique MBL et checkpoint

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

Tête avant ce checkpoint : `178320bb0305db298697b5b6800c628d78940535`. Les avancées dual-leveling et PLR décrites ci-dessous sont présentes dans l'historique. Ce journal est actualisé à chaque checkpoint.

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
- Audit de `M421_MBL` : `I/J` sont lus avec `parser.value_long()` et conservés en `int32_t`, puis contrôlés avant toute indexation de la grille. Les valeurs hors plage sont rejetées avec `STR_ERR_MESH_XY`, comme pour la grille ABL.
- La version EEPROM de la configuration a été incrémentée de `V88` à `V89`, car le schéma de stockage a changé.

**Conséquence au premier flash :** ne pas réutiliser aveuglément les anciens paramètres EEPROM. Charger les valeurs par défaut (`M502`), puis sauvegarder (`M500`) après vérification du démarrage et du comportement des axes. Les anciens décalages/profils peuvent devoir être restaurés manuellement à partir d'une sauvegarde préalable.

Ces jalons sont sauvegardés dans l'historique Git de la branche. L'état de compilation reste non vérifié.

## État réel — non validé matériellement

- La branche comporte un premier jeu de paramètres Mizar, mais ceux-ci sont encore préliminaires.
- Le noyau dual MBL/ABL et les écrans LVGL sont intégrés, mais ne sont pas encore compilés avec succès ni testés. Les séquences de retour/annulation nécessitent encore une revue complète.
- Le parcours EEPROM utilise les zones natives MBL et ABL distinctes, plus un booléen de sélection ; le schéma est marqué V89 et nécessite M502/M500 au premier démarrage.
- Le mapping runout doit être contrôlé avec les deux branches de pins (LVGL et non-LVGL) : l'écran tactile utilise PA4, donc PA4 ne doit jamais être affecté au capteur filament.
- Le circuit de détection de perte secteur utilise PE3 dans la configuration LVGL de la carte. L'état électrique réel et les pull-up/pull-down doivent être validés sur carte.
- La ressource TFT/LVGL et les langues françaises/anglaises ne sont pas encore réintégrées/validées.
- Les options de pause/runout et la reprise PLR ont été activées en première passe seulement ; leurs séquences doivent être auditées avant d'être déclarées correctes.
- La compilation PlatformIO a été exécutée par GitHub Actions; le build le plus récent a échoué dans `draw_autoLevel.cpp` sur `style_yellow_label` non déclaré. Aucun essai sur imprimante n'a été effectué.

## Checkpoint courant — 2026-11-07

- Branche distante vérifiée : `mizar-s-marlin-2.1.2.9`, tête avant ce checkpoint `8af51727ad382e6ef1c1b4062cda1a5e9677a031`. Aucun changement demandé ou appliqué à `main` ni à `bugfix-2.1.x`.
- `M421_MBL` lit les index via `parser.value_long()` dans un `int32_t` avant de vérifier `GRID_MAX_POINTS_X/Y`, ce qui évite le rétrécissement prématuré des grandes valeurs avant leur rejet.
- Les fichiers des écrans LVGL automatiques/manuels et les en-têtes de langues EN/FR sont déjà présents dans la branche. Leur parcours complet, l'association des chaînes de texte et les ressources binaires restent à auditer; leur présence seule ne prouve pas que les workflows ni les assets sont intégrés correctement.
- CI : le run `42391389321` pour `178320bb0305db298697b5b6800c628d78940535` a dépassé les erreurs précédentes puis échoué sur le symbole `style_yellow_label` manquant. Remplacé dans le checkpoint suivant par `style_sel_text`; build à relancer.
- Runout, reprise après coupure, PLR et comportement des callbacks LVGL restent non validés statiquement de bout en bout et matériellement.

## Prochaine séquence

1. Auditer les transitions/callbacks entre sélection ABL/MBL, écrans 4x4, retour et arrêt, puis corriger une séquence à la fois.
2. Auditer le runout avec pause/reprise d'impression, puis le parcours PLR et les interactions SD/EEPROM.
3. Vérifier quelles ressources MKS requises existent dans 2.1.2.9 et intégrer les assets/langues effectivement nécessaires.
4. Examiner les résultats du workflow CI et corriger les erreurs de compilation jusqu'à obtenir une build démontrée.
5. Documenter les commandes de build/flash, procédure EEPROM et validation sur banc; obtenir les essais matériels avant toute déclaration de validation.

## Règle de livraison

Ne jamais marquer une fonction comme « terminée » tant que la cohérence du code, sa compilation dans l'environnement visé et, lorsque nécessaire, un essai réel n'ont pas été réalisés.