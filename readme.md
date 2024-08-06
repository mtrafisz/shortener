# Shortener - Simple API written in C

## Running

To run the server, you need to build the project first. See the [Building](#building) section for more information.

After building the project, you can run the server by running the executable in root directory of the project.

```bash
./build/shortener
```

Executable expects `./static` directory to be present in current working directory.

## Usage - API

### `POST /short`

This endpoint is used to shorten a URL. It expects a JSON payload with a key `url` containing the URL to shorten.

#### Request content

In request body:

```json
{
    "url": "https://example.com/really/long/url"
}
```

#### Response content

On success:

`201 Created`

`application/json` with body:

```json
{
    "id": "random_id"
}
```

On error:

`400 Bad Request` - invalid json body (no url key, malformed json, etc.).

`500 Internal Server Error` - database related error.

`text/plain` - Body contains error message.

---

### `GET /short/{id}`

This endpoint is used to redirect to the original URL. It expects a path parameter `id` containing the shortened URL ID.

#### Request content

`id` - shortened URL ID. Can be obtained from `POST /short` response.

#### Response content

on success:

`302 Found` with header `Location: <original_url>`

on error:

`404 Not Found` - shortened URL ID not found.

`400 Bad Request` - invalid shortened URL ID (couldn't be parsed).

---

### `DELETE /short/{id}`

This endpoint is used to delete a shortened URL. It expects a path parameter `id` containing the shortened URL ID.

#### Request content

`id` - shortened URL ID. Can be obtained from `POST /short` response.

#### Response content

on success:

`204 No Content`

on error:

`400 Bad Request` - invalid shortened URL ID (couldn't be parsed).

`500 Internal Server Error` - database related error.

---

### `GET /`

This endpoint serves the static files from `./static` directory. It is used to serve the frontend.

## Building

I've tried to make this project, and its core - the [comet](https://github.com/mtrafisz/comet) framework, as portable as possible. It should work on any platform that supports CMake and C99, but I've only tested it on Linux in Mint variety.

This project depends on `sqlite3` dev library. It is not shipped with the project, nor is it included as a submodule. You need to have it installed on your system. On debian-likes, you can install it by running:

```bash
sudo apt install libsqlite3-dev
```

Intended building process uses *cmake*:

In root directory of the project run:

```bash
cmake -S . -B build && cmake --build build
```

If cmake complains about missing some files / libraries, run:

```bash
git submodule update --init --recursive
```

