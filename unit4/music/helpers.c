// Helper functions for music

#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "helpers.h"

// Converts a fraction formatted as X/Y to eighths
int duration(string fraction)
{
    int dur = (8 / ((int)fraction[2] - 48)) * ((int)fraction[0] - 48);
    return dur;
}

// Calculates frequency (in Hz) of a note
int frequency(string note)
{
    float num_note = 0.0, octo = 0.0, freq = 0.0;
    char cur_note[3] = {"\0"};
    bool b = false;

    const string NOTES[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    //split octo and note
    for (int i = 0, lenght = strlen(note); i < lenght; i++)
    {
        if (isdigit(note[i]))
        {
            octo = (float)note[i] - 48.0;
        }
        else
        {
            cur_note[i] = note[i];
            cur_note[i + 1] = '\0';
        }
    }

    //look for b in note
    if (cur_note[1] == 'b')
    {
        cur_note[1] = '\0';
        b = true;
    }

    //search number of note
    for (int j = 0; j < 12; j++)
    {
        if (strcmp(NOTES[j], cur_note) == 0 && !b)
        {
            num_note = j;
            break;
        }
        else if (strcmp(NOTES[j], cur_note) == 0 && b)
        {
            num_note = j - 1;
            break;
        }
    }

    freq = roundf(pow(2.0, (num_note - 9.0) / 12.0) * 440.0 * pow(2.0, octo - 4.0));

    return freq;
}

// Determines whether a string represents a rest
bool is_rest(string s)
{
    if (strcmp(s, "") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
