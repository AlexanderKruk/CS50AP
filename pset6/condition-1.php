<?php
    $n  = readline("I'd like an integer please: ");

    if($n > 0)
    {
         printf("You picke a positive number! \n");
    }
    else if ($n == 0)
    {
         printf("You picked zero!\n");
    }
    else
    {
         printf("You picked negative number!");
    }
?>