CREATE TABLE Movie(
    id INT NOT NULL PRIMARY KEY,
	title VARCHAR(100),
	year INT,
	rating VARCHAR(10),
	company VARCHAR(50)
) ENGINE = InnoDB;
# 1. Primary Key constraint on id


CREATE TABLE Actor(
	id INT NOT NULL PRIMARY KEY,
	last VARCHAR(20),
	first VARCHAR(20),
	sex VARCHAR(6),
	dob DATE,
	dod DATE,
	CONSTRAINT dod_dob_Actor CHECK((`dod` IS NOT NULL and `dob` IS NOT NULL and `dob` < `dod`) or `dod` IS NULL),
	CONSTRAINT validate_sex_Actor CHECK((`sex` IN ('Male','Female')))
) ENGINE = InnoDB;
# 2. Primary Key constraint on id
# 3. Check contraint on dates to make sure date of death is greater than date of birth
# 4. Check constarint on sex to ensure it is Male or Female.

CREATE TABLE Director(
	id INT NOT NULL PRIMARY KEY,
	last VARCHAR(20),
	first VARCHAR(20),
	dob DATE,
	dod DATE,
	CONSTRAINT dod_dob_Director CHECK((`dod` IS NOT NULL and `dob` IS NOT NULL and `dob` < `dod`) or `dod` IS NULL)
) ENGINE = InnoDB;
# 5. Primary Key constraint on id
# 6. Check contraint on dates to make sure date of death is greater than date of birth

CREATE TABLE MovieGenre(
	mid INT,
	genre VARCHAR(20),
	FOREIGN KEY (mid) REFERENCES Movie(id)
) ENGINE = InnoDB;
# 7. Referential Integrity constraint on mid

CREATE TABLE MovieDirector(
	mid INT,
	did INT,
	FOREIGN KEY (mid) REFERENCES Movie(id),
	FOREIGN KEY (did) REFERENCES Director(id)
) ENGINE = InnoDB;
# 8. Referential Integrity constraint on mid
# 9. Referential Integrity constraint on did

CREATE TABLE MovieActor(
	mid INT,
	aid INT,
	role VARCHAR(50),
	FOREIGN KEY (mid) REFERENCES Movie(id),
	FOREIGN KEY (aid) REFERENCES Actor(id)
) ENGINE = InnoDB;
# 10. Referential Integrity constraint on mid
# 11. Referential Integrity constraint on aid

CREATE TABLE Review(
    name VARCHAR(20),
	time TIMESTAMP,
	mid INT,
	rating INT,
	comment VARCHAR(500),
	UNIQUE(name, mid),
	FOREIGN KEY (mid) REFERENCES Movie(id),
	CHECK(`rating` >= 0 and `rating` <= 5)
) ENGINE = InnoDB;
# 12. Unique key constraint on name & Movie Id combined
# 13. Referential Integrity constraint on mid
# 14. Check constraint for rating, has to be greater than or equal to zero and less than or equal to five 

CREATE TABLE MaxPersonID(
	id INT PRIMARY KEY
) ENGINE = InnoDB;

CREATE TABLE MaxMovieID(
	id INT PRIMARY KEY
) ENGINE = InnoDB;