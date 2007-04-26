#!/usr/bin/php
<?php
$dom = new DOMDocument;
$dom->load( $argv[1] );
if ( $dom->validate() )
	echo "Validation passed!\n";
?>
