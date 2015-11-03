-- Query: Give me the names of all the actors in the movie 'Die Another Day'.
-- Please also make sure actor names are in this format:
-- <firstname> <lastname>   (seperated by single space).

-- Description: Perform an Inner Join on Actor and MovieActor tables on Actor.id and MovieActor.aid respectively
-- Perform an inner join of above result and Movie table on MovieActor.mid and Movie.id respectively
-- Where condition for the movie 'Die another Day' i.e. filtering the tuples
-- After this the actor's first and last names are concatenated with a space using the mysql CONCAT function
-- as a renamed column "Cast of the movie: Die Another Day"
SELECT CONCAT(first, ' ', last) AS `Cast of the movie: Die Another Day`
FROM Actor
INNER JOIN MovieActor ON Actor.id = MovieActor.aid
INNER JOIN Movie on MovieActor.mid = Movie.id
WHERE Movie.title = 'Die Another Day';

-- Query: Give me the count of all the actors who acted in multiple movies.
-- Description: INNER JOIN on Actor and MovieActor tables on attributes id and aid respectively
-- Group By results by Actor's ids
-- Filter results by Having clause keeping only those Actor ids whose count of MovieActor id is greater than 1
-- i.e. these actors have acted in multiple movies
SELECT COUNT(*)
FROM (
	SELECT Actor.id
	FROM Actor
	INNER JOIN MovieActor ON Actor.id = MovieActor.aid
	GROUP BY Actor.id
	HAVING COUNT(MovieActor.aid) > 1
) as tableAlias;

-- Query: Give the name of the Director for the movie named "Lord of the Rings"
-- Perform inner join on 3 tables to first get the movie id for "Lord of the rings series",
-- then use that movie id to fetch the director id and use this id to fetch Director name.
SELECT CONCAT(first," ",last) FROM Director WHERE id IN (SELECT did FROM MovieDirector,
Movie WHERE MovieDirector.mid = Movie.id AND title LIKE '%lord of the rings%');