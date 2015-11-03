# For all three of these, they violate the primary key of 'id' because there are duplicate ids:

INSERT INTO Movie VALUES (2, 'blah', 2015, 'blah', 'blah');
#Violation of Primary Key constraint:
#ERROR 1062 (23000): Duplicate entry '2' for key 'PRIMARY'

INSERT INTO Actor VALUES (10, 'blah', 'blah', 'blah', '1990-10-20', '2030-10-20');
#Violation of Primary Key constraint:
#ERROR 1062 (23000): Duplicate entry '10' for key 'PRIMARY'

INSERT INTO Director VALUES (2, 'blah', 'blah', '1990-10-20', '2030-10-20');
#Violation of Primary Key constraint:
#ERROR 1062 (23000): Duplicate entry '2' for key 'PRIMARY'

-- -----------------------------------------CHECK CONSTRAINT VIOLATION-------------------------
INSERT INTO Actor VALUES (10, 'blah', 'blah', 'M', '1990-10-20', '2030-10-20');
#Violation of check constraint:
#Sex of an Actor should only be specified as 'Male' or 'Female'.

INSERT INTO Actor(id, last, first, sex, dob, dod)
VALUES (65537, 'blah', 'blah', 'female', '1920-10-20', '1820-10-20');
#Violation of check constraint:
# Date of Death should be less that Date of Birth

INSERT INTO Review(name, time, mid, rating, comment)
VALUES ('Andy', 1445331544, 2, 10, 'Awesome movie');
#Violation of Check constraint:
#Rating must be greater than or equal to zero and less than or equal to five

-- --------------------------------------FOREIGN KEY VIOLATION--------------------------------

INSERT INTO MovieGenre(mid, did)
VALUES (0, "Comedy");
# Reference integrity constraint violation: mid in MovieGenre must reference a valid id in the Movie table
#ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))


INSERT INTO MovieDirector(mid, did)
VALUES (0, 1);
# Reference integrity constraint violation: mid in MovieDirector must reference a valid id in the Movie table
# ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

INSERT INTO MovieDirector(mid, did)
VALUES (2, 1);
# Reference integrity constraint violation: did in MovieDirector must reference a valid id in the Director table
# ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`MovieDirector`, CONSTRAINT `MovieDirector_ibfk_2` FOREIGN KEY (`did`) REFERENCES `Director` (`id`))

INSERT INTO MovieActor(mid, aid, role)
VALUES (1, 1, 'Passenger');
# Reference integrity constraint violation: mid in MovieActor must reference a valid tuple in the Movie table
# ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

INSERT INTO MovieActor(mid, aid, role)
VALUES (2, 0, 'Passenger');
# Reference integrity constraint violation: aid in MovieActor must reference a valid tuple in the Actor table
# ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`MovieActor`, CONSTRAINT `MovieActor_ibfk_2` FOREIGN KEY (`aid`) REFERENCES `Actor` (`id`))

INSERT INTO Review(name, time, mid, rating, comment)
VALUES ("Andy","1445331544", 0, 1, "Horrible movie");
# Reference integrity constraint violation: mid in Review must reference a valid tuple in the Movie table
#ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails
#(`CS143`.`Review`, CONSTRAINT `Review_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))

-- -------------------------UNIQUE KEY VIOLATION-----------------------------------------

INSERT INTO Review(name, time, mid, rating, comment)
VALUES ('blah', 1445331544, 2, 1, 'bad movie'), ('blah', 1445331544, 2, 4, 'good movie');
#Unique key constraint violation: combination of name and mid should be unique
#ERROR 1062 (23000): Duplicate entry 'blah-2' for key 'name'

-- ------------------------------------------------------------------------------------------
