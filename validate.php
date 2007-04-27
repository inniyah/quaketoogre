#!/usr/bin/php
<?php
if ( count( $argv ) < 2 )
{
	die( "Usage: validate.php [xml file]\n" );
}

$dom = new DOMDocument;
$dom->load( $argv[1] );
if ( $dom->validate() )
	echo "Validation passed!\n";
?>
