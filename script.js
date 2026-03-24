// --- Initialisation ---
const canvas = document.getElementById('jeuCanvas');
const ctx = canvas.getContext('2d');

// Variables pour stocker les données du serveur
let mapData = null;
let mapW = 0, mapH = 0, tileSize = 0;
let joueur = { x: 200, y: 200 };
let bot = { x: 100, y: 100 };

// --- Connexion WebSocket ---
const ws = new WebSocket('ws://localhost:8000');

ws.onopen = () => {
    console.log("Connecté au serveur !");
    // Dès qu'on est connecté, on demande la carte au serveur
    ws.send("INIT");
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);

    if (data.type === "map") {
        // Le serveur nous envoie la carte
        mapData = data.data;
        mapW = data.w;
        mapH = data.h;
        tileSize = data.size;
    } 
    else if (data.type === "state") {
        // Le serveur nous envoie les positions à 60 FPS
        joueur.x = data.x;
        joueur.y = data.y;
        bot.x = data.bx;
        bot.y = data.by;
        
        // On redessine tout
        dessinerJeu();
    }
};

// --- Fonction de Dessin ---
function dessinerJeu() {
    // 1. On efface tout le canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // 2. On dessine la carte (les murs)
    if (mapData) {
        ctx.fillStyle = "#2c3e50"; // Couleur des murs (gris foncé)
        for (let y = 0; y < mapH; y++) {
            for (let x = 0; x < mapW; x++) {
                let index = y * mapW + x;
                if (mapData[index] === 1) { // Si c'est un mur
                    ctx.fillRect(x * tileSize, y * tileSize, tileSize, tileSize);
                }
            }
        }
    }

    // 3. On dessine le Bot (la proie)
    ctx.fillStyle = "#f1c40f"; // Jaune
    ctx.beginPath();
    ctx.arc(bot.x, bot.y, 10, 0, Math.PI * 2);
    ctx.fill();

    // 4. On dessine le Joueur (le requin)
    ctx.fillStyle = "#e74c3c"; // Rouge
    ctx.beginPath();
    ctx.arc(joueur.x, joueur.y, 10, 0, Math.PI * 2);
    ctx.fill();
}

// --- Gestion des touches du clavier ---
// Quand on appuie sur une touche (+HAUT, +BAS...)
document.addEventListener('keydown', (e) => {
    if (e.key === "ArrowUp" || e.key === "z") ws.send("+HAUT");
    if (e.key === "ArrowDown" || e.key === "s") ws.send("+BAS");
    if (e.key === "ArrowLeft" || e.key === "q") ws.send("+GAUCHE");
    if (e.key === "ArrowRight" || e.key === "d") ws.send("+DROITE");
});

// Quand on relâche une touche (-HAUT, -BAS...)
document.addEventListener('keyup', (e) => {
    if (e.key === "ArrowUp" || e.key === "z") ws.send("-HAUT");
    if (e.key === "ArrowDown" || e.key === "s") ws.send("-BAS");
    if (e.key === "ArrowLeft" || e.key === "q") ws.send("-GAUCHE");
    if (e.key === "ArrowRight" || e.key === "d") ws.send("-DROITE");
});