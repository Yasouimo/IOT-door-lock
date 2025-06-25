<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
header('Content-Type: text/plain');

// Connexion à la base MySQL
$db = new mysqli('localhost', 'root', '', 'rfid_access');

// Vérifier si la connexion a échoué
if ($db->connect_error) {
    die("Erreur de connexion MySQL: " . $db->connect_error);
}

// Récupérer l'UID envoyé par l'ESP32 (via GET)
$uid = $_GET['uid'] ?? '';

if (empty($uid)) {
    die("DENIED"); // UID vide = accès refusé
}

// Vérifier si l'UID existe dans la table 'users'
$query = $db->prepare("SELECT id FROM users WHERE uid = ?");
$query->bind_param('s', $uid);
$query->execute();
$result = $query->get_result();

if ($result->num_rows > 0) {
    // UID trouvé : accès autorisé
    echo "GRANTED";

    // Enregistrer le log (accès réussi)
    $logQuery = $db->prepare("INSERT INTO logs (uid, access_status) VALUES (?, 'GRANTED')");
    $logQuery->bind_param('s', $uid);
    $logQuery->execute();
} else {
    // UID inconnu : accès refusé
    echo "DENIED";

    // Enregistrer le log (accès refusé)
    $logQuery = $db->prepare("INSERT INTO logs (uid, access_status) VALUES (?, 'DENIED')");
    $logQuery->bind_param('s', $uid);
    $logQuery->execute();
}

$db->close();
?>
