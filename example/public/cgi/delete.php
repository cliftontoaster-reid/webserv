<?php
// delete.php
// Expects POST with 'name' (the random file name, e.g. "abc123")
// Deletes: {name}, {name}._txt, {name}.webp (if exists)
// Returns JSON: { success: true/false, message: '...' }

$uploadDir = '/srv/upload/';  // must match $folderPath in index.php

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
  http_response_code(405);
  echo json_encode(['success' => false, 'message' => 'Method not allowed']);
  exit;
}

if (!isset($_POST['name']) || empty($_POST['name'])) {
  http_response_code(400);
  echo json_encode(['success' => false, 'message' => 'Missing file name']);
  exit;
}

$name = basename($_POST['name']); // prevent directory traversal
$basePath = $uploadDir . $name;
$filesToDelete = [
  $basePath,
  $basePath . '._txt',
  $basePath . '.webp',
];

$deleted = 0;
$errors = [];
foreach ($filesToDelete as $file) {
  if (file_exists($file)) {
    if (unlink($file)) {
      $deleted++;
    } else {
      $errors[] = "Could not delete " . basename($file);
    }
  }
}

if ($deleted === 0 && empty($errors)) {
  // No files found – maybe already deleted
  echo json_encode(['success' => true, 'message' => 'No files to delete']);
} elseif (empty($errors)) {
  echo json_encode(['success' => true, 'message' => "Deleted $deleted file(s)"]);
} else {
  http_response_code(500);
  echo json_encode(['success' => false, 'message' => implode(', ', $errors)]);
}
